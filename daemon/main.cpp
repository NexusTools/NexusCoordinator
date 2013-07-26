#include <QCoreApplication>
#include <QDebug>

#include "nexuscoordinator.h"
#include "nexusconfig.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("NexusCoordinator");
#ifdef GIT_REVISION
    a.setApplicationVersion(VER_MAJ "." VER_MIN "." GIT_REVISION " (" GIT_BRANCH ")");
#elif defined(VER_MAJ)
    a.setApplicationVersion(VER_MAJ "." VER_MIN " (Unknown Branch)");
#else
    a.setApplicationVersion("Unknown Version (Unknown Branch)");
#endif

    NexusCoordinator coordinator;
    NexusConfig configuration;

    qDebug() << "NexusCoordinator V" + a.applicationVersion();
    
    return a.exec();
}
