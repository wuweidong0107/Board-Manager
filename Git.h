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
        QByteArray result;
        int process_exit_code = 0;
    };
private:
	Private m;
public:
    struct Branch {
		QString name;
        bool is_current;
        bool operator==(const Branch& other) const {
            return name == other.name;
        }
        bool operator!=(const Branch& other) const {
            return name != other.name;
        }
    };
    
    Git(const QString &repodir);
    void setGitCommand(const QString &gitcmd, const QString &sshcmd = {});
	QString gitCommand() const;
    bool git(const QString &arg, bool chdir = true);
    bool chdirExec(std::function<bool()> fn);
    void clearResult();
    QString gitResult();
    QList<Branch> localBranches();
    
signals:
public:
    static bool isValidRepo(const QString &dir);
};

#endif // GIT_H
