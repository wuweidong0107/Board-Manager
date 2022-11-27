#include "Device.h"

Device::Device()
{
    
}

void Device::setId(const QString &id)
{
    m.id = id;
}

void Device::setName(const QString &name)
{
    m.name = name;
}

void Device::setAddress(const QHostAddress &address)
{
    m.address = address;
}

bool Device::operator==(const Device &other) const
{
    return m.id == other.id() 
            && m.name == other.id() 
            && m.address == other.address();
}

bool Device::operator!=(const Device &other) const
{
    return !((*this) == other);
}
