#include "httpserver.h"

#include <coordinator-macros.h>

HTTPServer::HTTPServer()
{
}

void HTTPServer::startImpl() {
    qDebug() << config<QVariantList>("Hosts");
}

void HTTPServer::stopImpl() {

}

BeginModule(HTTPServer, Service)
EndModule()
