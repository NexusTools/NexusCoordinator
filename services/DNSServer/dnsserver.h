#ifndef DNSSERVER_H
#define DNSSERVER_H

#include "dnsserver_global.h"

#include <coordinatorservice.h>

class DNSSERVERSHARED_EXPORT DNSServer : CoordinatorService
{
    Q_OBJECT

public:
    Q_INVOKABLE DNSServer();

protected:
    inline void startImpl() {}
    inline void stopImpl() {}

};

#endif // DNSSERVER_H
