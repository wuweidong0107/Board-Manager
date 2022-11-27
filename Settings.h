#ifndef SETTINGS_H
#define SETTINGS_H

#include <QHostAddress>
#include "Device.h"

class Settings
{
public:
    static Settings* instance() { return obj; }
    quint16 getBroadcastPort() const;
    quint16 getBroadcastInterval() const;
    void setBroadcastInterval(quint16 interval);
    void setBroadcastPort(quint16 port);    
    void saveSettings();
    
private:
    struct Private {
        quint16 BCPort{0};
        quint16 BCInterval{0};      
    };
    Private m;
    Settings();
    void loadSettings();
        
    static Settings* obj;
};

#endif // SETTINGS_H
