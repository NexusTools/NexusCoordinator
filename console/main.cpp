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

    {
        const char* msg = "Starting NexusCoordinator Console V" VERSION "\n\n";
        fwrite(msg, 1, strlen(msg), stdout);
        fflush(stdout);
        sleep(2);
    }

    CoordinatorConsole console;
    return a.exec();
}
