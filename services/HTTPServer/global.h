#ifndef HTTPSERVER_GLOBAL_H
#define HTTPSERVER_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef EXPORT_DECL
#  define EXPORTED Q_DECL_EXPORT
#else
#  define EXPORTED Q_DECL_IMPORT
#endif

#endif // HTTPSERVER_GLOBAL_H
