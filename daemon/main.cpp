#include <QCoreApplication>
#include <QMutexLocker>
#include <QDateTime>
#include <QtGlobal>
#include <QThread>
#include <QDebug>

#include <signal.h>

#include <nexusconfig.h>
#include <coordinatorservice.h>
#include <nexuscoordinator.h>
#include <project-version.h>

inline void readConfig() {
    qDebug() << "Reading configuration ...";
    try {
        NexusCoordinator::init(NexusConfig::parseFile("config.xml", NexusConfig::XmlFormat).toMap());
    } catch(const char* err) {
        qWarning() << err;
        qDebug() << "Using internal default configuration instead.";
        NexusCoordinator::init(NexusConfig::parseFile(":/default.xml", NexusConfig::XmlFormat).toMap());
    }
}

#ifdef Q_OS_LINUX
void handleSignal(int signum) {
    QDebug debug(QtDebugMsg);
    debug << "Signal received" << signum << "...";

    if(signum == SIGHUP) {
        debug << "Reloading";
        readConfig();
    } else {
        debug << "Quitting";
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

#ifdef LEGACY_QT
void __msgOutput(QtMsgType type, const char * msg)
#else
void __msgOutput(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
#endif
{
    QByteArray streamData;
    {
        QTextStream textStream(&streamData, QIODevice::WriteOnly);
        textStream << '[';
        textStream << QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
        textStream << "] [";
        {
            QObject* thread = QThread::currentThread();
            QByteArray name = thread->objectName().toLocal8Bit();
            if(name.isEmpty())
                name = thread->metaObject()->className();
            textStream << name;
            textStream << ' ';
            textStream << thread;
        }
        textStream << "] ";

#ifndef LEGACY_QT
        QString func(ctx.function);
        if(func.isEmpty())
            func = "anonymous";
        else {
            QRegExp methodName("[\\s^]([\\w]+[\\w\\d\\-_]*(::[\\w]+[\\w\\d\\-_]*)?)\\(");
            if(methodName.indexIn(ctx.function) > -1)
                func = methodName.cap(1);
        }
        textStream << '[';
        textStream << func;
        textStream << "] ";

        /* QString file(ctx.file);
        if(file.isEmpty())
            file = "Unknown";

        textStream << file << ':' << ctx.line << '\n'; */
#endif

        switch(type) {
            case QtDebugMsg:
                textStream << "[DEBUG]";
            break;

            case QtWarningMsg:
                textStream << "[WARNING]";
            break;

            case QtCriticalMsg:
                textStream << "[CRITICAL]";
            break;

            case QtFatalMsg:
                textStream << "[FATAL]";
            break;

            default:
                textStream << "[UNKNOWN]";
            break;
        }

        textStream << ' ' << msg << "\n";
    }


    // Ensure messages don't get merged due to threading
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    FILE* out = type != QtDebugMsg ? stderr : stdout;
    fwrite(streamData.data(), 1, streamData.length(), out);
    fflush(out);

    if(type == QtFatalMsg)
        abort();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName("NexusTools");
    a.setApplicationName("NexusCoordinator");
    a.setApplicationVersion(VERSION);

    // Custom message handler'
#ifdef LEGACY_QT
    qInstallMsgHandler(__msgOutput);
#else
    qInstallMessageHandler(__msgOutput);
#endif

    qDebug() << qPrintable("NexusCoordinator V" VERSION);

    readConfig();

#ifdef Q_OS_UNIX
    qDebug() << "Connecting signals ...";
    WATCH_SIGNAL(SIGHUP);

    WATCH_SIGNAL(SIGINT);
    WATCH_SIGNAL(SIGQUIT);

    WATCH_SIGNAL(SIGTRAP);
    WATCH_SIGNAL(SIGABRT);

    WATCH_SIGNAL(SIGTERM);
#endif // TODO: Windows error handling

    qDebug() << "Starting services ...";
    if(NexusCoordinator::instance()->services().isEmpty())
        qFatal("No services configured, please fix your configuration");

    foreach(CoordinatorService* srv, NexusCoordinator::instance()->services())
        srv->start();

    return a.exec();
}
