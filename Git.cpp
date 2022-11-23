#include "Git.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>

Git::Git(const QString &repodir)
{
    m.git_command = "git";
    m.ssh_command = "ssh";
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

QList<Git::Branch> Git::branches()
{
    QList<Branch> branches;
    Branch b;
    b.name = "MyBranch";
    branches.push_back(b);
    return branches;
}
