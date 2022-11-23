#include <QtWidgets>
#include "mainwindow.h"
#include "AddRepositoryDialog.h"
#include "Git.h"
#include "RepositoryBookmark.h"

static QString bookmark = "/home/wwd/.config/board-manager/bookmarks.xml";

struct MainWindow::Private {
    QList<RepositoryData> repos;
    RepositoryData current_repo;
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

void MainWindow::removeRepositoryFromBookmark(const RepositoryData &repo, bool ask)
{
    if (ask) {
		int r = QMessageBox::warning(this, tr("Confirm Remove"), tr("Are you sure you want to remove the repository from bookmarks?") + '\n' + tr("(Files will NOT be deleted)"), QMessageBox::Ok, QMessageBox::Cancel);
		if (r != QMessageBox::Ok) return;
	}
	auto *repos = getReposPtr();
    repos->removeAll(repo);   
    RepositoryBookmark::save(bookmark, &getRepos());
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
        item->setIcon(QIcon::fromTheme("folder"));
        m->current_repo = repo;
	}
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

void MainWindow::onAddPackage()
{
    AddRepositoryDialog dlg(this);
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

void MainWindow::on_packageList_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem *item = packageList->currentItem();
    if (!item) return;
    
    const RepositoryData repo = itemRepoMap.value(item);
    QMenu menu;
    QPoint pt = packageList->mapToGlobal(pos);
    QAction *a_open = menu.addAction(tr("&Open"));
    QAction *a_remove = menu.addAction(tr("&Remove"));
    QAction *a = menu.exec(pt + QPoint(8, -8));
    if (a) {
        if (a == a_open) {
            openPackage(repo);
            return;
        }
        if (a == a_remove) {
            removeRepositoryFromBookmark(repo, true);
            updatePackageList();
            if (repo != m->current_repo)
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

void MainWindow::createActions()
{
    packageMenu = menuBar()->addMenu(tr("&Package"));
    QAction *addPackageAct = new QAction(tr("&Add"), this);
    addPackageAct->setShortcuts(QKeySequence::Open);
    addPackageAct->setStatusTip(tr("Add a new package"));
    connect(addPackageAct, &QAction::triggered, this, &MainWindow::onAddPackage);
    packageMenu->addAction(addPackageAct);
    packageMenu->addSeparator();

    QAction *quitAct = packageMenu->addAction(tr("&Exit"), this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    boardMenu = menuBar()->addMenu(tr("&Board"));
    QAction *addBoardAct = new QAction(tr("&Add"), this);
    addBoardAct->setStatusTip(tr("Add a new board"));
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
    productList->addItems(QStringList()
            << "PROD_2092_DSB28"
            << "PROD_2123_MSM28");
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
