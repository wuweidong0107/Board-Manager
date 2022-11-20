#ifndef REPOSITORYBOOKMARK_H
#define REPOSITORYBOOKMARK_H

#include <QMetaType>

struct RepositoryData {
	QString name;
	QString path;
};
Q_DECLARE_METATYPE(RepositoryData);

class RepositoryBookmark
{
public:
    RepositoryBookmark();
    static bool save(QString const &path, QList<RepositoryData> const *item);
    static QList<RepositoryData> load(QString const &path);
};

#endif // REPOSITORYBOOKMARK_H
