#ifndef CONTROLSERVER_GLOBAL_H
#define CONTROLSERVER_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef EXPORT_DECL
#  define EXPORTED Q_DECL_EXPORT
#else
#  define EXPORTED Q_DECL_IMPORT
#endif

#endif // CONTROLSERVER_GLOBAL_H
