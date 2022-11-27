#include "Settings.h"
#include <QSettings>

const QString SETTINGS_FILE{"Board-Manager.ini"};
#define DefaultBroadcastPort        56780
#define DefaultBroadcastInterval    2000    // 2 secs

Settings* Settings::obj = new Settings;
quint16 Settings::getBroadcastPort() const
{
    return m.BCPort;
}

quint16 Settings::getBroadcastInterval() const
{
    return m.BCInterval;
}

void Settings::setBroadcastInterval(quint16 interval)
{
    m.BCInterval = interval;
}

void Settings::setBroadcastPort(quint16 port)
{
    if (port > 0)
        m.BCPort = port;
}

void Settings::saveSettings()
{
    QSettings settings(SETTINGS_FILE);
    settings.setValue("BroadcastPort", m.BCPort);
    settings.setValue("BroadcastInterval", m.BCInterval);
}

Settings::Settings()
{
    loadSettings();
}

void Settings::loadSettings()
{
    QSettings settings(SETTINGS_FILE);
    m.BCPort = settings.value("BroadcastPort", DefaultBroadcastPort).value<quint16>();
    m.BCInterval = settings.value("BroadcastInterval", DefaultBroadcastInterval).value<quint16>();    
}
