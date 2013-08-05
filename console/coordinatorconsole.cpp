#include "coordinatorconsole.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

void CoordinatorConsole::terminateRequested(int sig) {
    if(fork_rv > 0) {
        fork_rv = 0;

        _statusQueue << "Process terminated";
    } else
        CursesMainWindow::terminateRequested(sig);
}

void CoordinatorConsole::dropToShell() {
    fork_rv = fork();
    if (fork_rv == 0)
    {
        endwin();

        static QByteArray message("\e[H\e[2JYou have been dropped to a temporary shell.\nNexusCoordinator is still running, type 'exit' to return.\n\n");
        fwrite(message.data(), 1, message.length(), stdout);
        fflush(stdout);

        execl("/bin/bash", "bash", 0);

        _exit(1);
    }
    else if (fork_rv == -1)
    {
        // TODO: show error message
        return;
    }

    endwin();
    sleep(2);

    int status;
    while (-1 == waitpid(fork_rv, &status, 0));
    fork_rv=0;

    titleChanged();
    refresh();
}

void CoordinatorConsole::aptInstall(QString pkg) {
    fork_rv = fork();
    if (fork_rv == 0)
    {
        endwin();

        QByteArray message(QByteArray("\e[H\e[2JFollow the instructions below, they will help you install `") + pkg.toLocal8Bit().data() + "`.\n\n");
        fwrite(message.data(), 1, message.length(), stdout);
        fflush(stdout);

        execl("/usr/bin/sudo", "sudo", "/usr/bin/apt", "install", pkg.toLocal8Bit().data(), 0);

        _exit(1);
    }
    else if (fork_rv == -1)
    {
        // TODO: show error message
        return;
    }

    endwin();

    int status;
    while (fork_rv > 0 && waitpid(fork_rv, &status, 0));
    fork_rv=0;

    titleChanged();
    cursesDirtyMainWindow();
}


void CoordinatorConsole::dropToRootShell() {
    fork_rv = fork();
    if (fork_rv == 0)
    {
        endwin();

        static QByteArray message("\e[H\e[2JYou have been dropped to a temporary shell.\n\n");
        fwrite(message.data(), 1, message.length(), stdout);
        fflush(stdout);

        execl("/usr/bin/sudo", "sudo", "/bin/bash", 0);

        _exit(1);
    }
    else if (fork_rv == -1)
    {
        // TODO: show error message
        return;
    }

    endwin();

    int status;
    while (fork_rv > 0 && -1 == waitpid(fork_rv, &status, 0));
    fork_rv=0;

    titleChanged();
    cursesDirtyMainWindow();
}

