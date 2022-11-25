#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AddPackageDialog.h"
#include "RepositoryBookmark.h"
#include "Git.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
class QTreeWidgetItem;
class QListWidgetItem;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    const QList<RepositoryData> &getRepos() const;
	QList<RepositoryData> *getReposPtr();
    
private slots:
    void onAddPackage();
    void on_packageList_customContextMenuRequested(const QPoint &pos);
    void on_packageList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_packageList_itemDoubleClicked(QListWidgetItem *item);
private:
    void createActions();
    void createStatusBar();
    void createDockWindows();
    QString makeRepositoryName(const QString &path);
    void addRepositoryToBookmark(const RepositoryData &repo);
    void removeRepositoryFromBookmark(const RepositoryData &repo, bool ask);
    void updatePackageList();
    void openPackage(const RepositoryData &repo);
    void updateProductList(GitPtr g);
    void openProduct(const Git::Branch &b);
    void updateStatusBarText();
    GitPtr git(const QString &dir) const;

    QMenu *packageMenu;
    QMenu *boardMenu;
    QMenu *viewMenu;
    QDockWidget *packageDock;
    QDockWidget *productDock;
    QDockWidget *boardDock;
    QListWidget *packageList;
    QListWidget *productList;
    QListWidget *boardList;
    QMap<QListWidgetItem *, RepositoryData> itemRepoMap;
    QMap<QListWidgetItem *, Git::Branch> itemBranchMap;
    struct Private;
    Private *m;
};

#endif
