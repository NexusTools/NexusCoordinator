#include <QCoreApplication>
#include <project-version.h>

#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#include "coordinatorconsole.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName("NexusTools");
    a.setApplicationName("NexusCoordinator");
    a.setApplicationVersion(VERSION);

    CoordinatorConsole console;
    return a.exec();
}
