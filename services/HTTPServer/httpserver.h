#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "global.h"

#include <coordinatorservice.h>
#include <commservergroup.h>
#include <httpservercore.h>

class EXPORTED HTTPServer : CoordinatorService
{
    Q_OBJECT
public:
    Q_INVOKABLE HTTPServer();

protected:
    void startImpl();
    void stopImpl();

private:
    HTTPServerCore _httpServerCore;

};

#endif // HTTPSERVER_H
