#ifndef REPOSITORYBOOKMARK_H
#define REPOSITORYBOOKMARK_H

#include <QMetaType>
#include <QDebug>
struct RepositoryData {
	QString name;
	QString path;
    bool operator==(const RepositoryData& other) const {
        return name == other.name && path == other.path;
    }
    bool operator!=(const RepositoryData& other) const {
        return name != other.name || path != other.path;
    }
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
