#include "httpserver.h"

#include <coordinator-macros.h>
#include <modularcore.h>

//#include <commstream.h>

HTTPServer::HTTPServer()
{
}

void HTTPServer::startImpl() {
    /*
    foreach(QVariant value, config<QVariantList>("Hosts")) {
        QStringList listen;
        QVariantMap host = value.toMap();
        if(host.contains("port")) {
            if(host.contains("listen"))
                qWarning() << "Port attribute overrides listen, this functionality may change later.";

            listen << QString("*:%1").arg(host.value("port").toString());
        } else
            listen = host.value("listen", "*").toString().split(",");
        QString path = host.value("root", "/var/www").toString();
        QRegExp domain(host.value("domain", ".+").toString());
        if(!domain.isValid()) {
            qCritical() << "Could not configure VHost" << path << domain.pattern() << domain.errorString();
            continue;
        }
        qDebug() << host;

        try {
            _httpServerCore.configureVHost(domain, path, listen);
        } catch(QString err) {
            qCritical() << "Could not configure VHost:" << qPrintable(err);
        } catch(const char* err) {
            qCritical() << "Could not configure VHost:" << err;
        } catch(...) {
            qCritical() << "Could not configure VHost: Unknown exception thrown";
        }
    }
    */
}

void HTTPServer::stopImpl() {

}

BeginModule(HTTPServer, Service)
FinishModule()
