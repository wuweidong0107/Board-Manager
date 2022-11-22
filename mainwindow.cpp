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

void MainWindow::updateRepositoryBookmark(RepositoryData &item)
{
    if (item.path.isEmpty() || item.name.isEmpty())
        return;
    
    auto *repos = getReposPtr();
    bool done = false;
    for(auto &repo : *repos) {
        RepositoryData *p = &repo;
        if (item.path == p->path) {
            done = true;
            break;
        }
    }
    if (!done) {     
        repos->push_back(item);
    }
    RepositoryBookmark::save(bookmark, &getRepos());
}

void MainWindow::updatePackageList()
{
    auto *repos = getReposPtr();
	*repos = RepositoryBookmark::load(bookmark);
    packageList->clear();
    
    for(int i = 0; i < repos->size(); i++) {
        RepositoryData const &repo = repos->at(i);
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(repo.name);
        item->setIcon(QIcon::fromTheme("folder"));
        item->setData(IndexRole, i);
        packageList->addItem(item);
    }
}

int MainWindow::indexOfRepository(const QListWidgetItem *item) const
{
    if (!item) return -1;
	return item->data(IndexRole).toInt();
}

void MainWindow::removeRepositoryFromBookmark(int index, bool ask)
{
    if (ask) {
		int r = QMessageBox::warning(this, tr("Confirm Remove"), tr("Are you sure you want to remove the repository from bookmarks?") + '\n' + tr("(Files will NOT be deleted)"), QMessageBox::Ok, QMessageBox::Cancel);
		if (r != QMessageBox::Ok) return;
	}
	auto *repos = getReposPtr();
	if (index >= 0 && index < repos->size()) {
		repos->erase(repos->begin() + index);
		updatePackageList();
    }
}

void MainWindow::updateStatusBarText()
{
    QString text;
	
	QWidget *w = qApp->focusWidget();
	if (w == packageList) {
        RepositoryData const *repo = repositoryItem(packageList->currentItem());
        if (repo) {
            text = repo->path;
        }
	}
    statusBar()->showMessage(text);
}

void MainWindow::addRepository(const QString &dir)
{
    AddRepositoryDialog dlg(this, dir);
    if (dlg.exec() == QDialog::Accepted) {
        QString dir = dlg.path();
        if (!Git::isValidRepo(dir))
            return;
        RepositoryData item;
        item.path = dir;
        item.name = makeRepositoryName(dir);
        updateRepositoryBookmark(item);
    }
}

void MainWindow::onAddPackage()
{
    addRepository(QString());
    updatePackageList();
}

RepositoryData const *MainWindow::repositoryItem(QListWidgetItem const *item) const
{
    if (item) {
		int row = item->data(IndexRole).toInt();
		if (row < 0 && row >= getRepos().size()) {
			return nullptr;
		}
        QList<RepositoryData> const &repos = getRepos();
        return (row >= 0 && row < repos.size()) ? &repos[row] : nullptr;
	}
    return nullptr;
}

void MainWindow::on_packageList_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem *item = packageList->currentItem();
    if (!item) return;
    
    int index = indexOfRepository(item);
    QMenu menu;
    QAction *a_remove = menu.addAction(tr("&Remove"));
    QPoint pt = packageList->mapToGlobal(pos);
    QAction *a = menu.exec(pt);
    if (a) {
        if (a == a_remove) {
            removeRepositoryFromBookmark(index, true);
            return;
        }
    }
}

void MainWindow::on_packageList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    updateStatusBarText();
}

void MainWindow::on_packageList_itemDoubleClicked(QListWidgetItem *item)
{
    RepositoryData const *repo = repositoryItem(item);
    if (repo) {
        m->current_repo = *repo;
        for (int i = 0; i < packageList->count(); i++) {
            QListWidgetItem *item2 = packageList->item(i);
            QFont font = item2->font();
            if (item2 == item) {
                font.setBold(true);
                item2->setFont(font);
                item2->setData(OpenRole, true);
                item2->setIcon(QIcon::fromTheme("folder-open"));
            } else { 
                font.setBold(false);
                item2->setFont(font);
                item2->setData(OpenRole, false);
                item2->setIcon(QIcon::fromTheme("folder"));     // close
            }           
        }
	}
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
