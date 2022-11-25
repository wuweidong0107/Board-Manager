#include "Git.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QProcess>

Git::Git(const QString &repodir)
{
    m.git_command = "/usr/bin/git";
    m.ssh_command = "/usr/bin/ssh";
    m.working_repo_dir = repodir;
}

bool Git::isValidRepo(const QString &dir)
{
    return QFileInfo(dir).isDir() && QDir(dir + "/.git").exists();
}

void Git::setGitCommand(const QString &gitcmd, const QString &sshcmd)
{
    m.git_command = gitcmd;
    m.ssh_command = sshcmd;
}

QString Git::gitCommand() const
{
    return m.git_command;
}

bool Git::chdirExec(std::function<bool()> fn)
{
    bool res = false;
    QDir cwd = QDir::current();
    if (QDir::setCurrent(m.working_repo_dir)) {
        res = fn();
        QDir::setCurrent(cwd.path());
    }
    return res;
}
bool Git::git(const QString &arg, bool chdir)
{
    QFileInfo file(gitCommand());
    if(!file.isExecutable()) {
        qDebug() << "invalid git command: " << gitCommand();
        return false;
    }
#ifndef QT_NO_DEBUG
    qDebug() << "exec: git " << arg;
#endif
    clearResult();
    auto runGit = [&](){
        QString cmd = QString("%1 ").arg(gitCommand());
        cmd += arg;
        
        QProcess p;
        p.start(cmd);
        p.closeWriteChannel();
        p.setReadChannel(QProcess::StandardOutput);
        while (1) {
            QProcess::ProcessState s = p.state();
            if (p.waitForReadyRead(1)) {
                while (1) {
                    char tmp[1024];
                    qint64 len = p.read(tmp, sizeof(tmp));
                    if (len < 1) break;
                    m.result.append(tmp, len);
                }
            } else if (s == QProcess::NotRunning) {
                break;
            }
        }
        m.process_exit_code = p.exitCode();
#ifndef QT_NO_DEBUG
        qDebug() << QString("exit code: %1").arg(m.process_exit_code);
        if (m.process_exit_code !=0 ) {
            qDebug() << QString::fromUtf8(p.readAllStandardError());
        }
#endif
        return  m.process_exit_code;
    };
    
    if (chdir) {
        return chdirExec(runGit);
    } else {
        return runGit();
    }
}

void Git::clearResult()
{
    m.result.clear();
    m.process_exit_code = 0;
}

QString Git::gitResult()
{
    return QString::fromUtf8(m.result);
}

QList<Git::Branch> Git::localBranches()
{
    QList<Branch> branches;
    git(QString("branch -l"));
    QString result = gitResult();
#ifndef QT_NO_DEBUG
    qDebug() << "result: " << result;
#endif
    QStringList list = result.split("\n");
    for (const auto &i : list) {
        if (!i.isEmpty()) {
            Branch b;
            b.is_current = i.startsWith("*") ? true:false;
            b.name = i.mid(2);
            branches.push_back(b);            
        }
    }
    return branches;
}

void Git::checkoutBranch(const QString &name)
{
    git("checkout " + name);
}
