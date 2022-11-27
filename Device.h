#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QtNetwork/QHostAddress>

class Device
{
public:
    Device();
    inline QString id() const { return m.id; }
    inline QString name() const { return m.name; };
    inline QHostAddress address() const { return m.address; }
    
    void setId(const QString& id);
    void setName(const QString& name);
    void setAddress(const QHostAddress& address);
    
    bool operator==(const Device& other) const;
    bool operator!=(const Device& other) const;
    
private:
    struct Private {
        QString id{""};
        QString name{""};
        QHostAddress address{QHostAddress::Null};
    };
    Private m;
};

#endif // BOARD_H
