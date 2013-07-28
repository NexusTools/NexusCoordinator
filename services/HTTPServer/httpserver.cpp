#include "httpserver.h"

#include <coordinator-macros.h>

HTTPServer::HTTPServer()
{
}

void HTTPServer::startImpl() {
    foreach(QVariant value, config<QVariantList>("Hosts")) {
        QVariantMap host = value.toMap();
        qDebug() << host;
    }
}

void HTTPServer::stopImpl() {

}

BeginModule(HTTPServer, Service)
EndModule()
