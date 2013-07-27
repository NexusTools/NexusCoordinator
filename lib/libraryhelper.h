#ifndef LIBRARYHELPER_H
#define LIBRARYHELPER_H

#include <QByteArray>
#include <QDebug>
#include <QFile>

#ifdef GIT_REVISION
#define VERSION VER_MAJ "." VER_MIN "." GIT_REVISION " (" GIT_BRANCH ")"
#elif defined(VER_MAJ)
#define VERSION VER_MAJ "." VER_MIN " (Unknown Branch)"
#else
#define VERSION "Unknown Version (Unknown Branch)"
#endif


#define DeclareCoordinatorLibrary(Class, Type) \
    extern "C" Class* NexusCoordinator_##Type##_##Class##_CreateInstance() { \
        return new Class();\
    } \
    \
    extern "C" QByteArray NexusCoordinator_##Type##_##Class##_MetaData() { \
        QFile file(":/" #Type "s/" #Class "/library.xml"); \
        qDebug() << file.fileName(); \
        if(file.open(QFile::ReadOnly)) { \
            return file.readAll(); \
        } else \
            return QByteArray(); \
    } \
    \
    extern "C" const char* NexusCoordinator_##Type##_##Class##_Version() { \
        return VERSION; \
    }

#endif // LIBRARYHELPER_H