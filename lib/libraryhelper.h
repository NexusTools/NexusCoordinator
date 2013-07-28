#ifndef LIBRARYHELPER_H
#define LIBRARYHELPER_H

#include <project-version.h>

#include <QByteArray>
#include <QDebug>
#include <QFile>

#ifndef EXPORTED
#error "EXPORTED macro required, see built-in services for reference."
#endif

#define DeclareCoordinatorLibrary(Class, Type) \
    extern "C" EXPORTED Class* NexusCoordinator_##Type##_##Class##_CreateInstance() { \
        return new Class(); \
    } \
    \
	extern "C" EXPORTED QByteArray NexusCoordinator_##Type##_##Class##_MetaData() { \
        QFile file(":/" #Type "s/" #Class "/library.xml"); \
        qDebug() << file.fileName(); \
        if(file.open(QFile::ReadOnly)) { \
            return file.readAll(); \
        } else \
            return QByteArray(); \
    } \
    \
    extern "C" EXPORTED const char* NexusCoordinator_##Type##_##Class##_Authors() { \
        return AUTHORS; \
    } \
    \
    extern "C" EXPORTED const char* NexusCoordinator_##Type##_##Class##_Version() { \
        return VERSION; \
    }

#endif // LIBRARYHELPER_H
