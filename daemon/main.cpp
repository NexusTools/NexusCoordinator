#include <QCoreApplication>
#include <QDebug>

#include <signal.h>

#include <nexuscoordinator.h>
#include <nexusconfig.h>

NexusCoordinator coordinator;

inline void reloadConfig() {
    coordinator.loadConfig(NexusConfig::parseFile("config.xml", NexusConfig::XmlFormat).toMap());
}

void handleSignal(int signum) {
    qDebug() << "Signal received" << signum;

    if(signum == SIGHUP)
        reloadConfig();
    else {
        qDebug() << "Quitting";
        QCoreApplication::instance()->quit();
    }
}

void connectSignal (int __sig, __sighandler_t __handler) {
    qDebug() << __sig << (void*)__handler;
    if(signal(__sig, __handler) == SIG_ERR)
        qWarning() << "Failed to connect signal";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("NexusCoordinator");
    a.setOrganizationName("NexusTools");

#ifdef GIT_REVISION
    a.setApplicationVersion(VER_MAJ "." VER_MIN "." GIT_REVISION " (" GIT_BRANCH ")");
#elif defined(VER_MAJ)
    a.setApplicationVersion(VER_MAJ "." VER_MIN " (Unknown Branch)");
#else
    a.setApplicationVersion("Unknown Version (Unknown Branch)");
#endif

    qDebug() << QString("NexusCoordinator V" + a.applicationVersion()).toLocal8Bit().data();


    try {
        reloadConfig();
    } catch(const char* err) {
        qWarning() << err;
        qDebug() << "Using internal default configuration instead.";
        coordinator.loadConfig(NexusConfig::parseFile(":/default.xml", NexusConfig::XmlFormat).toMap());
    }

    qDebug() << "Connecting signals...";
    connectSignal(SIGHUP, handleSignal);
    connectSignal(SIGQUIT, handleSignal);
    connectSignal(SIGINT, handleSignal);
    connectSignal(SIGABRT, handleSignal);
    connectSignal(SIGKILL, handleSignal);

    qDebug() << "Starting services...";
    return a.exec();
}
