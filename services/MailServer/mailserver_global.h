#ifndef MAILSERVER_GLOBAL_H
#define MAILSERVER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MAILSERVER_LIBRARY)
#  define MAILSERVERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MAILSERVERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MAILSERVER_GLOBAL_H
