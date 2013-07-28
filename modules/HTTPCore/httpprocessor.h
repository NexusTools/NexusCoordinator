#ifndef HTTPPROCESSOR_H
#define HTTPPROCESSOR_H

#include "global.h"

#include <commpacketprocessor.h>

class HTTPCORESHARED_EXPORT HTTPProcessor : CommPacketProcessor
{
public:
    Q_INVOKABLE HTTPProcessor();

    void processData(QByteArray) {}

};

#endif // HTTPPROCESSOR_H
