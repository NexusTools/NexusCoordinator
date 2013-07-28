#ifndef CONTROLSERVER_H
#define CONTROLSERVER_H

#include "global.h"

#include "coordinatorservice.h"

class EXPORTED ControlServer : CoordinatorService
{
    Q_OBJECT
public:
    Q_INVOKABLE ControlServer();

protected:
    inline void reloadConfigImpl() {}
    inline void startImpl() {}
    inline void stopImpl() {}
};

#endif // CONTROLSERVER_H
