#ifndef FTPSERVER_H
#define FTPSERVER_H

#include "ftpserver_global.h"

#include <coordinatorservice.h>

class FTPSERVERSHARED_EXPORT FTPServer : CoordinatorService
{
    Q_OBJECT
public:
    Q_INVOKABLE FTPServer();

    inline void startImpl() {}
    inline void stopImpl() {}

};

#endif // FTPSERVER_H
