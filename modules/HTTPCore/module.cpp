#include <coordinator-macros.h>

#include <QRectF>

#include "httpservercore.h"
#include "httprequesthandler.h"

BeginModuleNamespace(HTTPCore, Module)
DeclarePlugin(HTTPServerCore)
DeclarePlugin(HTTPRequestHandler)
DeclarePlugin(HTTPFileRequestHandler)
FinishModule()
