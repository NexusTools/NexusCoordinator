#ifndef HTTPPROCESSOR_H
#define HTTPPROCESSOR_H

#include "httpabstractprocessor.h"

class HTTPCORESHARED_EXPORT HTTPProcessor : HTTPAbstractProcessor
{
public:
    Q_INVOKABLE HTTPProcessor();

    virtual void processData(QByteArray);

};

#endif // HTTPPROCESSOR_H
