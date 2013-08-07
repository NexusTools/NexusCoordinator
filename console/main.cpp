#include <QCoreApplication>
#include <project-version.h>

#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#include <QDateTime>
#include <QThread>
#include <QDebug>
#include <QDir>

void __msgOutput(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    QByteArray streamData;
    {
        QTextStream textStream(&streamData, QIODevice::WriteOnly);
        textStream << '[';
        textStream << QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
        textStream << "] [";
        {
            QThread* thread = QThread::currentThread();
            QByteArray name = thread->objectName().toLocal8Bit();
            if(name.isEmpty())
                name = thread->metaObject()->className();
            textStream << name;
            textStream << ' ';
            textStream << thread;
        }
        textStream << "] [";

        switch(type) {
            case QtDebugMsg:
                textStream << "DEBUG";
            break;

            case QtWarningMsg:
                textStream << "WARNING";
            break;

            case QtCriticalMsg:
                textStream << "CRITICAL";
            break;

            case QtFatalMsg:
                textStream << "FATAL";
            break;

            default:
                textStream << "UNKNOWN";
            break;
        }

        textStream << "] ";

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

        QString file(ctx.file);
        if(file.isEmpty())
            file = "Unknown";

        textStream << file << ':' << ctx.line << '\n';
        textStream << msg << "\n";
    }


    // Ensure messages don't get merged due to threading
    static QFile log("output.log");
    if(!log.isOpen())
        log.open(QFile::WriteOnly);
    log.write(streamData);
    log.flush();
}

#include "coordinatorconsole.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName("NexusTools");
    a.setApplicationName("NexusCoordinator");
    a.setApplicationVersion(VERSION);

    qInstallMessageHandler(__msgOutput);

    char* path = getenv("PATH");
    if(path)
        QDir::setSearchPaths("bin", QString::fromLocal8Bit(path).split(':'));
    else
        QDir::setSearchPaths("bin", QStringList() << "/usr/bin" << "/bin" << "/local/bin" << "/local/usr/bin" << "/opt/bin" << "/opt/usr/bin");

    new CoordinatorConsole();
    int ret = a.exec();
    endwin();

    return ret;
}
