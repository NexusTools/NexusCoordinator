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

QString CoordinatorConsole::quoteArg(QString arg) {
    QString ar;
    QString arS;
    bool space = false;
    bool needQuote = false;
    foreach(QChar c, arg) {
        if(c.isSpace())
            space = true;
        else if(!c.isLetterOrNumber() && c != '-') {
            needQuote = true;
            arS.clear();

            if(!c.isPunct())
                ar += '\\';
        }

        ar += c;
        if(!needQuote) {
            if(c.isSpace())
                arS += '\\';
            arS += c;
        }
    }

    if(needQuote)
        return QString("\"%1\"").arg(ar);
    else if(space)
        return arS;

    return ar;
}

void CoordinatorConsole::startShell(QStringList args, QByteArray startMsg, QString finMsg) {
    _terminated = false;

    QString quotedCmd;
    QByteArray binaryPath;
    QString binary = args.first();
    foreach(QString arg, args) {
        if(!quotedCmd.isEmpty())
            quotedCmd += ' ';

        quotedCmd += quoteArg(arg);
    }
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

        printf("\033]0;%s\007\e[H\e[2J", qPrintable(quotedCmd));
        fflush(stdout);

        QByteArray launchMsg = QString("Launching `%1` ...\n").arg(quotedCmd).toLocal8Bit();
        fwrite(launchMsg.data(), 1, launchMsg.length(), stdout);
        fflush(stdout);

        if(!startMsg.isEmpty()) {
            usleep(300000);
            printf("\e[H\e[2J");
            fwrite(startMsg.data(), 1, startMsg.length(), stdout);
            fflush(stdout);
        } else
            usleep(600000);
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
        printf("\n\nFailed to launch...");
        fflush(stdout);
        sleep(1);

        _exit(1);
    }
    else if (child_pid == -1)
    {
        _statusQueue << QString("Failed to launch `%1`").arg(quotedCmd);
        beep();
        return;
    }

    endwin();
    sleep(2);

    int status;
    while (child_pid > 0 && -1 == waitpid(child_pid, &status, 0));
    if(status != 0) {
        if(_terminated)
            _statusQueue << QString("`%1` terminated").arg(quotedCmd);
        else
            _statusQueue << QString("`%1` crashed").arg(quotedCmd);
        beep();
    } else if(!finMsg.isEmpty())
        _statusQueue << finMsg;
    else
        _statusQueue << QString("`%1` exited").arg(quotedCmd);
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

