#ifndef HTTPPACKET_H
#define HTTPPACKET_H

#include <commpacket.h>

#include <QHash>

class HTTPPacket// : public RawCommPacket
{
public:
    Q_INVOKABLE inline explicit HTTPPacket(QByteArray payload =QByteArray()) {}// : RawCommPacket(payload) {}

private:
    uint _status;
    QString _requestUri;

    QHash<QByteArray, QByteArray> _headers;
    QHash<QByteArray, QByteArray> _post;
    QHash<QByteArray, QByteArray> _get;

};

#endif // HTTPPACKET_H
