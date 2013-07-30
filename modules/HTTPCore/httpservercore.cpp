#include "httpservercore.h"

HTTPServerCore::HTTPServerCore()
{
}

vhostptr HTTPServerCore::configureVHost(QRegExp, QString, QStringList, RequestHandler*)
{
    return 0;
}
