#ifndef FTPSERVER_GLOBAL_H
#define FTPSERVER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FTPSERVER_LIBRARY)
#  define FTPSERVERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define FTPSERVERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // FTPSERVER_GLOBAL_H
