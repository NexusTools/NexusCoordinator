#ifndef HTTPSERVERCORE_H
#define HTTPSERVERCORE_H

#include <QStringList>
#include <QRegExp>

#include "httpprocessor.h"

class RequestProcessor;
class RequestHandler;
class QMetaObject;

typedef quintptr vhostptr;

class HTTPServerCore// : public CommServerGroup
{
public:
    Q_INVOKABLE HTTPServerCore();

    vhostptr configureVHost(QRegExp domain, QString path =QString("/var/www"), QStringList listen =QStringList(QStringList() << "*"), RequestHandler* =0);

    void setEnabled(vhostptr, bool =true);
    bool isRunning(vhostptr);

private:
    struct VHost {
        bool enabled;
        QRegExp domain;
        QStringList listeners;
        RequestHandler* handler;
    };

    //QHash<QRegExp, VHost*> _hosts;
    //QMap<vhostptr, VHost*> _hostMap;
    //QHash<QString, HTTPAbstractProcessor*> _protocolHandlers;

    static QHash<QString, QMetaObject*> _knownProtocolHandlers;
};

#endif // HTTPSERVERCORE_H
