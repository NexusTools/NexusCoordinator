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

void CoordinatorConsole::startShell(QStringList args, QByteArray message) {
    _terminated = false;

    QByteArray binaryPath;
    QString binary = args.first();
    {
        QFileInfo binaryInfo(QString("bin:%1").arg(binary));
        if(!binaryInfo.exists()) {
            _statusQueue << QString("Unable to locate `%1`").arg(binary);
            beep();
            return;
        }
        binaryPath = binaryInfo.filePath().toLocal8Bit();
    }

    fork_rv = fork();
    if (fork_rv == 0)
    {
        endwin();

        fwrite(message.data(), 1, message.length(), stdout);
        fflush(stdout);
        initEnv();

        char* rawPath = new char[binaryPath.size()+1];
        strcpy(rawPath, binaryPath.data());

        int i = 0;
        char** rawArgs = new char*[args.length()+1];
        for(; i<args.length(); i++) {
            rawArgs[i] = new char[args[i].length()+1];
            strcpy(rawArgs[i], args[i].toLocal8Bit().data());
        }
        rawArgs[i] = 0;

        execv(rawPath, rawArgs);

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
            _statusQueue << QString("Process `%1` terminated").arg(binary);
        else
            _statusQueue << QString("Process `%1` crashed").arg(binary);
        beep();
    }
    fork_rv=0;

    titleChanged();
    refresh();
}

void CoordinatorConsole::aptInstall(QString pkg) {
    startShell(QStringList() << "apt-get" << "install" << pkg, "\e[H\e[2JFollow the instructions below, they will help you install the required software.\n\n");
}

void CoordinatorConsole::dropToShell() {
    startShell(QStringList() << "bash", "\e[H\e[2JYou have been dropped to a temporary shell.\nNexusCoordinator is still running, type 'exit' to return.\n\n");
}

void CoordinatorConsole::dropToRootShell() {
    startShell(QStringList() << "sudo" << "bash", "\e[H\e[2JYou have been dropped to a temporary shell.\n\n");
}

