#ifndef CONTROLSERVER_H
#define CONTROLSERVER_H

#include <commserver.h>
#include <coordinatorservice.h>

#include "global.h"
#include "controlpacket.h"

class EXPORTED ControlServer : CoordinatorService
{
    Q_OBJECT
public:
    Q_INVOKABLE ControlServer();

protected:
    inline void reloadConfigImpl() {}
    inline void startImpl() {}
    inline void stopImpl() {}

private:
    //CommServerGroup<ControlClient, ControlPacketProcessor> _serverGroup;
};

#endif // CONTROLSERVER_H
