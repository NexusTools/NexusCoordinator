#ifndef HTTPABSTRACTPROCESSOR_H
#define HTTPABSTRACTPROCESSOR_H

#include "global.h"

class HTTPPacket;

class HTTPCORESHARED_EXPORT HTTPAbstractProcessor
{
protected:
    HTTPAbstractProcessor();

    void emitPacket(HTTPPacket*) {}
};

#endif // HTTPABSTRACTPROCESSOR_H
