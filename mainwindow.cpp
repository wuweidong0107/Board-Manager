#include <QtWidgets>
#include "mainwindow.h"
#include "AddRepositoryDialog.h"

MainWindow::MainWindow()
{
    createActions();
    createStatusBar();
    createDockWindows();

    setMinimumWidth(1280);
    setCentralWidget(boardDock);
    setWindowTitle(tr("Board-Manager"));
}

void MainWindow::addRepository(const QString &dir)
{
    AddRepositoryDialog dlg(this, dir);
    if (dlg.exec() == QDialog::Accepted) {
        
    }
}

void MainWindow::onAddPackage()
{
    addRepository(QString());
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
    addDockWidget(Qt::LeftDockWidgetArea, packageDock);
    viewMenu->addAction(packageDock->toggleViewAction());

    productDock = new QDockWidget(tr("Products"), this);
    productList = new QListWidget(productDock);
    productList->addItems(QStringList()
            << "PROD_2092_DSB28"
            << "PROD_2123_MSM28");
    productDock->setWidget(productList);
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
