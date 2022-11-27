#include "DeviceBroadcaster.h"
#include "Settings.h"
#include "Device.h"
#include <QJsonObject>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <QJsonDocument>
#include <QUuid>

struct DeviceBroadcaster::Private {
    Device thisDevice;
    QUdpSocket udpSock;
    QTimer timer;
    int BCMsgLength;
};

DeviceBroadcaster::DeviceBroadcaster(QObject *parent) 
    : QObject(parent),
      m(new Private)
{
    m->thisDevice.setId(QUuid::createUuid().toString());
    m->thisDevice.setName(QHostInfo::localHostName());
    connect(&m->timer, &QTimer::timeout, this, &DeviceBroadcaster::sendBroadcast);
    connect(&m->udpSock, &QUdpSocket::readyRead, this, &DeviceBroadcaster::processBroadcast);
    m->udpSock.bind(Settings::instance()->getBroadcastPort(), QUdpSocket::ShareAddress);
}

void DeviceBroadcaster::start()
{
    sendBroadcast();
    if (!m->timer.isActive())
        m->timer.start(Settings::instance()->getBroadcastInterval());
}

DeviceBroadcaster::~DeviceBroadcaster()
{
    delete m;
}

void DeviceBroadcaster::sendBroadcast()
{
    const quint16 port = Settings::instance()->getBroadcastPort();
    QJsonObject obj(QJsonObject::fromVariantMap({
                                                    {"id", m->thisDevice.id()},
                                                    {"name", m->thisDevice.name()},
                                                }));
    qDebug()<<__func__<<m->thisDevice.id()<<m->thisDevice.name();
    m->BCMsgLength = 2;
    QVector<QHostAddress> addresses = getBroadcastAddressFromInterfaces();
    QByteArray data(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    foreach (QHostAddress address, addresses) {
        m->udpSock.writeDatagram(data, address, port);
    }
}

void DeviceBroadcaster::processBroadcast()
{
    while (m->udpSock.hasPendingDatagrams()) {
        qDebug()<<__func__;
        QByteArray data;
        quint64 datagramSize =  m->udpSock.pendingDatagramSize();
        data.resize(datagramSize);
        
        QHostAddress sender;
        m->udpSock.readDatagram(data.data(), data.size(), &sender);
        QJsonObject obj = QJsonDocument::fromJson(data).object();
        if (obj.keys().length() == m->BCMsgLength) {
            Device device;
            device.setId(obj.value("id").toString());
            device.setName(obj.value("name").toString());
            qDebug()<<__func__<<device.id()<<device.name();
            emit broadcastReceived(device);
        }
    }
}

QVector<QHostAddress> DeviceBroadcaster::getBroadcastAddressFromInterfaces()
{
    QVector<QHostAddress> addresses;
    foreach (QNetworkInterface iface, QNetworkInterface::allInterfaces()) {
        if (iface.flags() & QNetworkInterface::CanBroadcast) {
            foreach (QNetworkAddressEntry addressEntry, iface.addressEntries()) {
                if (!addressEntry.broadcast().isNull()) {
                    addresses.push_back(addressEntry.broadcast());
                }
            }
        }
    }
    return addresses;
}
