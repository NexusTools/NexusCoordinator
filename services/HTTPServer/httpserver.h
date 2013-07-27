#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "global.h"

#include <coordinatorservice.h>

class EXPORTED HTTPServer : CoordinatorService
{
    
public:
    HTTPServer();

protected:
    inline void reloadConfigImpl() {}
    inline void startImpl() {}
    inline void stopImpl() {}

};

#endif // HTTPSERVER_H
