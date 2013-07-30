#ifndef HTTPREQUESTHANDLER_H
#define HTTPREQUESTHANDLER_H

#include <QVariantMap>
#include <QObject>

class HTTPPacket;

class HTTPRequestHandler : public QObject
{
    Q_OBJECT

    friend class HTTPServerCore;
protected:
    inline HTTPRequestHandler() {}

    virtual void handle(HTTPPacket&) =0;

    void serveGenericResponse(HTTPPacket&, quint16 code =404);

public:
    virtual QVariantMap toMap() =0;
    virtual void fromMap(QVariantMap) =0;

};

class HTTPFileRequestHandler : public HTTPRequestHandler
{
    Q_OBJECT

    friend class HTTPServerCore;
public:
    Q_INVOKABLE HTTPFileRequestHandler(QString rootPath =QString("/var/www"));

    virtual QVariantMap toMap();
    virtual void fromMap(QVariantMap);

protected:
    virtual void handle(HTTPPacket&);


};

#endif // HTTPREQUESTHANDLER_H
