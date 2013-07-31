#include <QCoreApplication>
#include <QtGlobal>
#include <QDebug>

#include <signal.h>

#include <nexusconfig.h>
#include <coordinatorservice.h>
#include <nexuscoordinator.h>
#include <project-version.h>

inline void reloadConfig() {
    NexusCoordinator::init(NexusConfig::parseFile("config.xml", NexusConfig::XmlFormat).toMap());
}

#ifdef Q_OS_LINUX
void handleSignal(int signum) {
    qDebug() << "Signal received" << signum;

    if(signum == SIGHUP)
        reloadConfig();
    else {
        qDebug() << "Quitting";
        QCoreApplication::instance()->quit();
    }
}

void connectSignal (int __sig, __sighandler_t __handler, const char* name) {
    qDebug() << __sig << (void*)__handler << name;
    if(signal(__sig, __handler) == SIG_ERR)
        qWarning() << "Failed to connect signal";
}

#define WATCH_SIGNAL(SIG) connectSignal(SIG, handleSignal, #SIG)
#endif

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName("NexusTools");
    a.setApplicationName("NexusCoordinator");
    a.setApplicationVersion(VERSION);

    qDebug() << qPrintable("NexusCoordinator V" VERSION);


    try {
        reloadConfig();
    } catch(const char* err) {
        qWarning() << err;
        qDebug() << "Using internal default configuration instead.";
        NexusCoordinator::init(NexusConfig::parseFile(":/default.xml", NexusConfig::XmlFormat).toMap());
    }

#ifdef Q_OS_UNIX
    qDebug() << "Connecting signals...";
    WATCH_SIGNAL(SIGHUP);

    WATCH_SIGNAL(SIGINT);
    WATCH_SIGNAL(SIGQUIT);

    WATCH_SIGNAL(SIGTRAP);
    WATCH_SIGNAL(SIGABRT);

    WATCH_SIGNAL(SIGTERM);
#endif // TODO: Windows error handling

    qDebug() << "Starting services...";
    if(NexusCoordinator::instance()->services().isEmpty())
        qFatal("No services loaded, please fix your configuration");

    foreach(CoordinatorService* srv, NexusCoordinator::instance()->services())
        srv->start();

    return a.exec();
}
