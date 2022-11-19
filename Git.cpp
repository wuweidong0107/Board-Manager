#include "Git.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>

Git::Git(QObject *parent) : QObject(parent)
{
    
}

bool Git::isValidRepo(const QString &dir)
{
    return QFileInfo(dir).isDir() && QDir(dir + "/.git").exists();
}
