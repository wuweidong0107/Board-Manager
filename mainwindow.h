#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AddRepositoryDialog.h"
#include "RepositoryBookmark.h"
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
public:
	enum {
		IndexRole = Qt::UserRole,
	};
    
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
    void updateRepositoryBookmark(RepositoryData &item);
    void updatePackageList();
    void openPackage(const RepositoryData &repo);
    int indexOfRepository(const QListWidgetItem *item) const;
    void removeRepositoryFromBookmark(int index, bool ask);
    void updateStatusBarText();
    RepositoryData const *repositoryItem(const QListWidgetItem *item) const;
    
    QMenu *packageMenu;
    QMenu *boardMenu;
    QMenu *viewMenu;
    QDockWidget *packageDock;
    QDockWidget *productDock;
    QDockWidget *boardDock;
    QListWidget *packageList;
    QListWidget *productList;
    QListWidget *boardList;
    QMap<QListWidgetItem *,RepositoryData> itemRepoMap;
    
    struct Private;
    Private *m;
};

#endif
