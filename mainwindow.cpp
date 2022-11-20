#include <QtWidgets>
#include "mainwindow.h"
#include "AddRepositoryDialog.h"
#include "Git.h"
#include "RepositoryBookmark.h"

static QString bookmark = "/home/wwd/.config/board-manager/bookmarks.xml";

struct MainWindow::Private {
    QList<RepositoryData> repos;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m(new Private)
    
{
    createActions();
    createStatusBar();
    createDockWindows();

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
    qDebug()<<__func__<<path;
    auto i = path.lastIndexOf('/');
    if (i >= 0) {
        i++;
        auto j = path.size();
        return path.mid(i, j-i);
    }
    return QString();
}

void MainWindow::saveRepositoryBookmark(RepositoryData &item)
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
        packageList->addItem(repo.name);
    }
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
        saveRepositoryBookmark(item);
    }
}

void MainWindow::onAddPackage()
{
    addRepository(QString());
    updatePackageList();
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
    packageList->addItems(QStringList()
            << "RK3568"
            << "RK3566"
            << "RK3328"
            << "RK3288");
    packageDock->setWidget(packageList);
    packageDock->setMinimumSize(QSize(300,300));
    addDockWidget(Qt::LeftDockWidgetArea, packageDock);
    viewMenu->addAction(packageDock->toggleViewAction());

    productDock = new QDockWidget(tr("Products"), this);
    productList = new QListWidget(productDock);
    productList->addItems(QStringList()
            << "PROD_2092_DSB28"
            << "PROD_2123_MSM28");
    productDock->setWidget(productList);
    productDock->setMinimumSize(QSize(300,300));
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
