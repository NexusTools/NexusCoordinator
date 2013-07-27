#include "httpserver.h"
#include "libraryhelper.h"


HTTPServer::HTTPServer()
{
}

void HTTPServer::startImpl() {
    qDebug() << config<QVariantList>("Hosts");
}

void HTTPServer::stopImpl() {

}

DeclareCoordinatorLibrary(HTTPServer, Service)
