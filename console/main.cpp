#include <QCoreApplication>
#include <project-version.h>

#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#include <QDebug>
#include <QDir>

#include "coordinatorconsole.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName("NexusTools");
    a.setApplicationName("NexusCoordinator");
    a.setApplicationVersion(VERSION);

    char* path = getenv("PATH");
    if(path)
        QDir::setSearchPaths("bin", QString::fromLocal8Bit(path).split(':'));
    else
        QDir::setSearchPaths("bin", QStringList() << "/usr/bin" << "/bin" << "/local/bin" << "/local/usr/bin" << "/opt/bin" << "/opt/usr/bin");

    CoordinatorConsole console;
    int ret = a.exec();
    endwin();

    return ret;
}
