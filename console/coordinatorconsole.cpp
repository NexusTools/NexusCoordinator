#include "coordinatorconsole.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

void CoordinatorConsole::dropToShell() {
    int fork_rv = fork();
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

    titleChanged();
    refresh();
}


void CoordinatorConsole::dropToRootShell() {
    int fork_rv = fork();
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
    sleep(2);

    int status;
    while (-1 == waitpid(fork_rv, &status, 0));

    titleChanged();
    refresh();
}

