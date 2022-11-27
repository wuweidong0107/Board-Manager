#ifndef DEVICEBROADCASTER_H
#define DEVICEBROADCASTER_H

#include <QObject>
#include <QtNetwork>
#include <QTimer>
#include "Device.h"

class DeviceBroadcaster : public QObject
{
    Q_OBJECT
public:
    explicit DeviceBroadcaster(QObject *parent = nullptr);
    void start();
     ~DeviceBroadcaster() override;
    
private slots:
    void sendBroadcast();
    void processBroadcast();
    
private:
    struct Private;
    Private *m;
    QVector<QHostAddress> getBroadcastAddressFromInterfaces();
    
signals:
    void broadcastReceived(const Device& fromDevice);
};

#endif // DEVICEBROADCASTER_H
