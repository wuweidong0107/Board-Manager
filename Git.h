#ifndef GIT_H
#define GIT_H

#include <QObject>
#include <memory>

class Git;
using GitPtr = std::shared_ptr<Git>;

class Git : public QObject
{
    struct Private {
        QString git_command;
        QString ssh_command;
        QString working_repo_dir;
    };
private:
	Private m;
public:
    struct Branch {
		QString name;
    };
    
    Git(const QString &repodir);
    void setGitCommand(QString const &gitcmd, const QString &sshcmd = {});
	QString gitCommand() const;
    QList<Branch> branches();
    
signals:
public:
    static bool isValidRepo(const QString &dir);
};

#endif // GIT_H
