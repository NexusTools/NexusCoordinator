#include "coordinatorconsole.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

#include <QDir>

#define SCREEN_DIR "/var/run/screen"

void killChildAtExit() {
    ((CoordinatorConsole*)CursesMainWindow::current())->killChild();
}

void CoordinatorConsole::killChild() {
    kill(child_pid, SIGTERM);
    kill(child_pid, SIGKILL);
}

void CoordinatorConsole::rescanAvailableFunctions() {
    // Rescan Launchers
    bool v = QFileInfo("bin:vim").exists();
    bool n = QFileInfo("bin:nano").exists();
    launchVim.setVisible(v);
    launchNano.setVisible(n || (!n && !v));
    launchNano.setText(n ? "Nano" : "Install Nano");

    bool w = QFileInfo("bin:w3m").exists();
    bool e = QFileInfo("bin:elinks").exists();
    bool l = QFileInfo("bin:lynx").exists();
    launchW3M.setVisible(w);
    launchELinks.setVisible(e || (!e && !w && !l));
    launchELinks.setText(e ? "ELinks" : "Install ELinks");
    launchLynx.setVisible(l);

    _launch.fitToContent();
    _launch.markDirty();

    // Rescan Screens
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
    if(child_pid > 0) {
        _terminated = true;
        kill(child_pid, sig);
    } else
        CursesMainWindow::terminateRequested(sig);
}

void initEnv() {
    signal(SIGHUP, SIG_ERR);
    signal(SIGSEGV, SIG_ERR);
    signal(SIGABRT, SIG_ERR);
    signal(SIGQUIT, SIG_ERR);
    signal(SIGINT, SIG_ERR);

    signal(SIGWINCH, SIG_IGN);
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

    child_pid = fork();
    if (child_pid == 0)
    {
        endwin();

        if(message.isEmpty())
            message = QString("Launching `%1`").arg(args.join(" ")).toLocal8Bit();
        printf("\e[H\e[2J");
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
    else if (child_pid == -1)
    {
        _statusQueue << QString("Failed to launch `%1`").arg(args.join(" "));
        beep();
        return;
    }

    static bool atExitHooked = false;
    if(!atExitHooked) {
        atexit(killChildAtExit);
        atExitHooked = true;
    }

    endwin();
    sleep(2);

    int status;
    while (child_pid > 0 && -1 == waitpid(child_pid, &status, 0));
    if(status != 0) {
        if(_terminated)
            _statusQueue << QString("Process `%1` terminated").arg(args.join(" "));
        else
            _statusQueue << QString("Process `%1` crashed").arg(args.join(" "));
        beep();
    }
    child_pid=0;

    rescanAvailableFunctions();
    titleChanged();
    refresh();
}

void CoordinatorConsole::aptUpdateUpgrade() {
    startShell(QStringList() << "sudo" << "bash" << "-c" << "apt-get update; apt-get upgrade", "Follow the instructions below.\n\n");
}

void CoordinatorConsole::aptUpdateDistUpgrade() {
    startShell(QStringList() << "sudo" << "bash" << "-c" << "apt-get update; apt-get dist-upgrade", "Follow the instructions below.\n\n");
}

void CoordinatorConsole::aptInstall(QString pkg) {
    startShell(QStringList() << "apt-get" << "install" << pkg, "Follow the instructions below, they will help you install the required software.\n\n");
}

void CoordinatorConsole::sudoReboot() {
    startShell(QStringList() << "sudo" << "reboot", "Enter your password to reboot.\n\n");
}

void CoordinatorConsole::dropToShell() {
    startShell(QStringList() << "bash", "You have been dropped to a temporary shell.\nNexusCoordinator is still running, type 'exit' to return.\n\n");
}

void CoordinatorConsole::dropToRootShell() {
    startShell(QStringList() << "sudo" << "bash", "You have been dropped to a temporary shell.\n\n");
}

void CoordinatorConsole::editCronTab() {
    startShell(QStringList() << "crontab" << "-e");
}


void CoordinatorConsole::runVim(QString file){
    QStringList args;
    args << "vim";
    if(!file.isEmpty())
        args << file;

    startShell(args);
}

void CoordinatorConsole::runNano(QString file){
    if(launchNano.text() == "Install Nano") {
        aptInstall("nano");
        return;
    }

    QStringList args;
    args << "nano";
    if(!file.isEmpty())
        args << file;

    startShell(args);
}

void CoordinatorConsole::runELinks(QString url){
    if(launchNano.text() == "Install ELinks") {
        aptInstall("elinks");
        return;
    }

    QStringList args;
    args << "elinks";
    if(!url.isEmpty())
        args << url;

    startShell(args);
}
void CoordinatorConsole::runLynx(QString url){
    QStringList args;
    args << "lynx";
    if(!url.isEmpty())
        args << url;

    startShell(args);
}
void CoordinatorConsole::runW3M(QString url){
    QStringList args;
    args << "w3m";
    if(!url.isEmpty())
        args << url;

    startShell(args);
}

