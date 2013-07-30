#ifndef MAILSERVER_H
#define MAILSERVER_H

#include "mailserver_global.h"

#include <coordinatorservice.h>

class MAILSERVERSHARED_EXPORT MailServer : CoordinatorService
{
    Q_OBJECT
public:
    Q_INVOKABLE MailServer();

    inline void startImpl() {}
    inline void stopImpl() {}

};

#endif // MAILSERVER_H
