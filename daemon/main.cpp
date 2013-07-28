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

void connectSignal (int __sig, __sighandler_t __handler) {
    qDebug() << __sig << (void*)__handler;
    if(signal(__sig, __handler) == SIG_ERR)
        qWarning() << "Failed to connect signal";
}
#endif

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("NexusCoordinator");
    a.setOrganizationName("NexusTools");
    a.setApplicationVersion(VERSION);

    qDebug() << QString("NexusCoordinator V" + a.applicationVersion()).toLocal8Bit().data();


    try {
        reloadConfig();
    } catch(const char* err) {
        qWarning() << err;
        qDebug() << "Using internal default configuration instead.";
        NexusCoordinator::init(NexusConfig::parseFile(":/default.xml", NexusConfig::XmlFormat).toMap());
    }

#ifdef Q_OS_UNIX
    qDebug() << "Connecting signals...";
    connectSignal(SIGHUP, handleSignal);
    connectSignal(SIGQUIT, handleSignal);
    connectSignal(SIGINT, handleSignal);
    connectSignal(SIGABRT, handleSignal);
    connectSignal(SIGTRAP, handleSignal);
    connectSignal(SIGTERM, handleSignal);
#endif // TODO: Windows error handling

    qDebug() << "Starting services...";
    if(NexusCoordinator::instance()->services().isEmpty())
        qFatal("No services loaded, please fix your configuration");

    foreach(CoordinatorService* srv, NexusCoordinator::instance()->services())
        srv->start();

    return a.exec();
}
