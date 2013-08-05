#include "coordinatorconsole.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#include <QDir>

#define SCREEN_DIR "/var/run/screen"

void CoordinatorConsole::rescanScreens() {
    QDir screenDir(SCREEN_DIR);
    if(screenDir.exists()) {
        screenListSeparator.show();
        installScreen.hide();
        createScreen.show();

    } else {
        screenListSeparator.hide();
        installScreen.show();
        createScreen.hide();

    }
    _screens.fitToContent();
    _screens.markDirty();
}

void CoordinatorConsole::terminateRequested(int sig) {
    if(fork_rv > 0)
        _terminated = true;
    else
        CursesMainWindow::terminateRequested(sig);
}

void initEnv() {
    signal(SIGWINCH, SIG_IGN);

    signal(SIGSEGV, SIG_ERR);
    signal(SIGABRT, SIG_ERR);
    signal(SIGTERM, SIG_ERR);

    signal(SIGKILL, SIG_ERR);
    signal(SIGQUIT, SIG_ERR);
    signal(SIGINT, SIG_ERR);
}

void CoordinatorConsole::dropToShell() {
    _terminated = false;

    fork_rv = fork();
    if (fork_rv == 0)
    {
        endwin();

        static QByteArray message("\e[H\e[2JYou have been dropped to a temporary shell.\nNexusCoordinator is still running, type 'exit' to return.\n\n");
        fwrite(message.data(), 1, message.length(), stdout);
        fflush(stdout);
        initEnv();

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
    while (fork_rv > 0 && -1 == waitpid(fork_rv, &status, 0));
    if(status != 0) {
        if(_terminated)
            _statusQueue << "Process terminated";
        else
            _statusQueue << "Process crashed";
    }
    fork_rv=0;

    titleChanged();
    refresh();
}

void CoordinatorConsole::aptInstall(QString pkg) {
    _terminated = false;

    fork_rv = fork();
    if (fork_rv == 0)
    {
        endwin();

        QByteArray message(QByteArray("\e[H\e[2JFollow the instructions below, they will help you install `") + pkg.toLocal8Bit().data() + "`.\n\n");
        fwrite(message.data(), 1, message.length(), stdout);
        fflush(stdout);
        initEnv();

        execl("/usr/bin/sudo", "sudo", "/usr/bin/apt-get", "install", pkg.toLocal8Bit().data(), 0);
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
    if(status != 0) {
        if(_terminated)
            _statusQueue << "Process terminated";
        else
            _statusQueue << "Process crashed";
    }
    fork_rv=0;

    titleChanged();
    cursesDirtyMainWindow();
    rescanScreens();
}


void CoordinatorConsole::dropToRootShell() {
    _terminated = false;

    fork_rv = fork();
    if (fork_rv == 0)
    {
        endwin();

        static QByteArray message("\e[H\e[2JYou have been dropped to a temporary shell.\n\n");
        fwrite(message.data(), 1, message.length(), stdout);
        fflush(stdout);
        initEnv();

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
    if(status != 0) {
        if(_terminated)
            _statusQueue << "Process terminated";
        else
            _statusQueue << "Process crashed";
    }
    fork_rv=0;

    titleChanged();
    cursesDirtyMainWindow();
}

