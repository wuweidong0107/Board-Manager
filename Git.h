#ifndef GIT_H
#define GIT_H

#include <QObject>

class Git : public QObject
{
public:
    explicit Git(QObject *parent = nullptr);
    
signals:
public:
    static bool isValidRepo(QString const &dir);
};

#endif // GIT_H
