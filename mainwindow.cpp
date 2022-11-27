#include <QtWidgets>
#include "mainwindow.h"
#include "AddPackageDialog.h"
#include "AddBoardDialog.h"
#include "Git.h"
#include "RepositoryBookmark.h"
#include "DeviceBroadcaster.h"

static QString bookmark = "/home/wwd/.config/board-manager/bookmarks.xml";

struct MainWindow::Private {
    QList<RepositoryData> repos;
    RepositoryData current_repo;
    Git::Branch current_branch;
    DeviceBroadcaster* broadcaster;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m(new Private)
    
{
    createActions();
    createStatusBar();
    createDockWindows();

    QFont font;
    font.setPointSize(11);
    setFont(font);
    setMinimumWidth(1280);
    setCentralWidget(boardDock);
    setWindowTitle(tr("Board-Manager"));
    updatePackageList();
    
    m->broadcaster = new DeviceBroadcaster(this);
    m->broadcaster->start();
}

const QList<RepositoryData> &MainWindow::getRepos() const
{
	return m->repos;
}

QList<RepositoryData> *MainWindow::getReposPtr()
{
	return &m->repos;
}

QString MainWindow::makeRepositoryName(const QString &path)
{
    auto i = path.lastIndexOf('/');
    if (i >= 0) {
        i++;
        auto j = path.size();
        return path.mid(i, j-i);
    }
    return QString();
}

void MainWindow::addRepositoryToBookmark(const RepositoryData &repo)
{
    if (repo.path.isEmpty() || repo.name.isEmpty())
        return;
    
    auto *repos = getReposPtr();
    bool found = false;
    for(auto &i : *repos) {
        RepositoryData *p = &i;
        if (repo.path == p->path) {
            found = true;
            break;
        }
    }
    if (!found) {     
        repos->push_back(repo);
    }
    RepositoryBookmark::save(bookmark, &getRepos());
}

bool MainWindow::removeRepositoryFromBookmark(const RepositoryData &repo, bool ask)
{
    if (ask) {
		int r = QMessageBox::warning(this, tr("Confirm Remove"), tr("Are you sure you want to remove this package ?") + '\n' + tr("(Files will NOT be deleted)"), QMessageBox::Ok, QMessageBox::Cancel);
		if (r != QMessageBox::Ok) return false;
	}
	auto *repos = getReposPtr();
    repos->removeAll(repo);   
    RepositoryBookmark::save(bookmark, &getRepos());
    return true;
}

void MainWindow::updatePackageList()
{
    auto *repos = getReposPtr();
	*repos = RepositoryBookmark::load(bookmark);
    packageList->clear();
    itemRepoMap.clear();
    
    for(int i = 0; i < repos->size(); i++) {
        const RepositoryData &repo = repos->at(i);
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(repo.name);
        item->setIcon(QIcon::fromTheme("folder"));
        packageList->addItem(item);
        itemRepoMap.insert(item, repo);
    }
}

void MainWindow::openPackage(const RepositoryData &repo)
{   
    if (m->current_repo == repo)
        return;

    QListWidgetItem *last_item = itemRepoMap.key(m->current_repo);
    if (last_item) {      
        QFont font = last_item->font();
        font.setBold(false);
        last_item->setFont(font);
        last_item->setIcon(QIcon::fromTheme("folder"));
    }
    
    QListWidgetItem *item = itemRepoMap.key(repo);
    if (item) {
        packageList->setCurrentItem(item);
        QFont font = item->font();
        font.setBold(true);
        item->setFont(font);
        item->setIcon(QIcon::fromTheme("folder-open"));
        m->current_repo = repo;
	}
    
    GitPtr g = git(repo.path);
    updateProductList(g);
    openProduct(m->current_branch);
}

void MainWindow::updateProductList(GitPtr g)
{
    productList->clear();
    itemBranchMap.clear();
    
    if (g != nullptr) {
        QList<Git::Branch> branches = g->localBranches();
        for (const Git::Branch &b : branches) {
            QListWidgetItem *item = new QListWidgetItem();
            item->setText(b.name);
            item->setIcon(QIcon::fromTheme("folder"));
            productList->addItem(item);
            itemBranchMap.insert(item, b);
            if (b.is_current) {
                openProduct(b);
            }
        }
    }
}

void MainWindow::openProduct(const Git::Branch &b)
{
    if (m->current_branch == b)
        return;

    QListWidgetItem *last_item = itemBranchMap.key(m->current_branch);
    if (last_item) {      
        QFont font = last_item->font();
        font.setBold(false);
        last_item->setFont(font);
        last_item->setIcon(QIcon::fromTheme("folder"));
    }
    
    QListWidgetItem *item = itemBranchMap.key(b);
    if (item) {
        productList->setCurrentItem(item);
        QFont font = item->font();
        font.setBold(true);
        item->setFont(font);
        item->setIcon(QIcon::fromTheme("folder-open"));
        m->current_branch = b;
	}
    GitPtr g = git(m->current_repo.path);
    g->checkoutBranch(b.name);
}

void MainWindow::updateStatusBarText()
{
    QString text;
	
	QWidget *w = qApp->focusWidget();
	if (w == packageList) {
        const RepositoryData &repo = itemRepoMap.value(packageList->currentItem());
        text = repo.path;
	}
    statusBar()->showMessage(text);
}

GitPtr MainWindow::git(const QString &dir) const
{
    GitPtr g = std::make_shared<Git>(dir);
    if (g && QFileInfo(g->gitCommand()).isExecutable()) {
		return g;
	} else {
		return GitPtr();
	}
}

void MainWindow::on_addPackage()
{
    AddPackageDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QString dir = dlg.path();
        if (!Git::isValidRepo(dir))
            return;
        RepositoryData item;
        item.path = dir;
        item.name = makeRepositoryName(dir);
        addRepositoryToBookmark(item);
        updatePackageList();
        openPackage(item);
    }
}

void MainWindow::on_addBoard()
{
    AddBoardDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        qDebug()<<__func__;
    }
}

void MainWindow::openExplorer(const RepositoryData &repo)
{
    QString dir = repo.path;
    if (QFileInfo(dir).isDir()) {
        QString scheme = "file://";
        QString url = scheme + dir;
        QDesktopServices::openUrl(url);
    }
}

void MainWindow::on_packageList_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem *item = packageList->currentItem();
    if (!item) return;
    
    const RepositoryData repo = itemRepoMap.value(item);
    QMenu menu;
    QPoint pt = packageList->mapToGlobal(pos);
    QAction *a_open = menu.addAction(tr("&Open"));
    QAction *a_open_folder = menu.addAction(tr("Open &Folder"));
    QAction *a_remove = menu.addAction(tr("&Remove"));
    QAction *a = menu.exec(pt + QPoint(8, -8));
    if (a) {
        if (a == a_open) {
            openPackage(repo);
            return;
        }
        if (a == a_open_folder) {
            openExplorer(repo);
            return;
        }
        if (a == a_remove) {
            if (removeRepositoryFromBookmark(repo, true) == false)
                return;
            updateProductList(nullptr); // clear prodcut list
            updatePackageList();
            openPackage(m->current_repo);
            return;
        }
    }
}

void MainWindow::on_packageList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    updateStatusBarText();
}

void MainWindow::on_packageList_itemDoubleClicked(QListWidgetItem *item)
{
    openPackage(itemRepoMap.value(item));
}

void MainWindow::on_productList_itemDoubleClicked(QListWidgetItem *item)
{
    openProduct(itemBranchMap.value(item));
}

void MainWindow::createActions()
{
    packageMenu = menuBar()->addMenu(tr("&Package"));
    QAction *addPackageAct = new QAction(tr("&Add"), this);
    addPackageAct->setShortcuts(QKeySequence::Open);
    addPackageAct->setStatusTip(tr("Add a new package"));
    connect(addPackageAct, &QAction::triggered, this, &MainWindow::on_addPackage);
    packageMenu->addAction(addPackageAct);
    packageMenu->addSeparator();

    QAction *quitAct = packageMenu->addAction(tr("&Exit"), this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    boardMenu = menuBar()->addMenu(tr("&Board"));
    QAction *addBoardAct = new QAction(tr("&Add"), this);
    addBoardAct->setStatusTip(tr("Add a new board"));
    connect(addBoardAct, &QAction::triggered, this, &MainWindow::on_addBoard);  
    boardMenu->addAction(addBoardAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createDockWindows()
{
    packageDock = new QDockWidget(tr("Packages"), this);
    packageDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    packageList = new QListWidget(packageDock);
    packageList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(packageList, &QListWidget::customContextMenuRequested, 
            this, &MainWindow::on_packageList_customContextMenuRequested);
    connect(packageList, &QListWidget::currentItemChanged,
            this, &MainWindow::on_packageList_currentItemChanged);
    connect(packageList, &QListWidget::itemDoubleClicked,
            this, &MainWindow::on_packageList_itemDoubleClicked);
    packageDock->setWidget(packageList);
    packageDock->setMinimumSize(QSize(400, 400));
    addDockWidget(Qt::LeftDockWidgetArea, packageDock);
    viewMenu->addAction(packageDock->toggleViewAction());
    
    productDock = new QDockWidget(tr("Products"), this);
    productList = new QListWidget(productDock);
    connect(productList, &QListWidget::itemDoubleClicked,
            this, &MainWindow::on_productList_itemDoubleClicked);
    productDock->setWidget(productList);
    productDock->setMinimumSize(QSize(400, 400));
    addDockWidget(Qt::LeftDockWidgetArea, productDock);
    viewMenu->addAction(productDock->toggleViewAction());
    
    boardDock = new QDockWidget(tr("Boards"), this);
    boardList = new QListWidget(boardDock);
    boardList->addItems(QStringList()
            << "Board1"
            << "Board2");
    boardDock->setWidget(boardList);
    addDockWidget(Qt::RightDockWidgetArea, boardDock);
    viewMenu->addAction(boardDock->toggleViewAction());
}
