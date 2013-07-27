#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "global.h"

#include <coordinatorservice.h>
#include <commservergroup.h>
#include "httpprocessor.h"

class EXPORTED HTTPServer : CoordinatorService
{
public:
    HTTPServer();

protected:
    void startImpl();
    void stopImpl();

private:
    CommServerGroup listenGroup;
    HTTPProcessor processor;

};

#endif // HTTPSERVER_H
