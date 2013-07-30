#ifndef HTTPABSTRACTPROCESSOR_H
#define HTTPABSTRACTPROCESSOR_H

#include "global.h"

#include <commpacketprocessor.h>

class HTTPPacket;

class HTTPCORESHARED_EXPORT HTTPAbstractProcessor : CommPacketProcessor
{
protected:
    HTTPAbstractProcessor();

    void emitPacket(HTTPPacket* httpPacket) {
        emit packetParsed(CommPacketRef((CommPacket*)httpPacket));
    }
};

#endif // HTTPABSTRACTPROCESSOR_H
