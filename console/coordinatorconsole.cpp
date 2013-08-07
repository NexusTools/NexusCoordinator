#include "coordinatorconsole.h"
#include "screenaction.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

#include <QCoreApplication>
#include <QDir>

#define SCREEN_DIR "/var/run/screen"

CoordinatorConsole::CoordinatorConsole(bool shellMode) : CursesMainWindow(
            shellMode ? QString("NexusCoordinator Shell on %1").arg(readHostname()) : QString("NexusCoordinator V%1").arg(QCoreApplication::instance()->applicationVersion())), _menuBar(this),
            _coordinator("Coord_inator", this), _launch("_Launch", this), _screens("Scree_ns", this), _system("S_ystem", this), _help("_Help", this),
            launchVim("Vim", &_launch), launchNano("Nano", &_launch), launchW3M("W3M", &_launch), launchELinks("ELinks", &_launch), launchLynx("Lynx", &_launch),
            createScreen("Create Screen", &_screens), installScreen("Install Screen", &_screens), screenListSeparator(&_screens), screenNoInstancesMessage(" No Active Screens ", &_screens), _statusBar(this) {

    _updateDateTime.setInterval(1000);
    connect(&_updateDateTime, SIGNAL(timeout()), this, SLOT(updateStatusMessage()));
    _updateDateTime.start();

    _blinkTimer.setInterval(600);
    connect(&_blinkTimer, SIGNAL(timeout()), this, SLOT(blinkStatus()));
    connect(&_statusBar, SIGNAL(clicked()), this, SLOT(notifyClicked()));

    CursesAction* action;
    if(shellMode) {
        action = new CursesAction("_Drop to Shell", &_coordinator);
        connect(action, SIGNAL(activated()), this, SLOT(dropToShell()), Qt::QueuedConnection);
        action = new CursesAction("Drop to _Root Shell", &_coordinator);
        connect(action, SIGNAL(activated()), this, SLOT(dropToRootShell()), Qt::QueuedConnection);

        _coordinator.addSeparator();
    }

    action = new CursesAction("Connect to...", &_coordinator);
    action->disable();
    action = new CursesAction("Disconnect", &_coordinator);
    action->disable();

    _coordinator.addSeparator();

    action = new CursesAction("E_xit", &_coordinator);
    connect(action, SIGNAL(activated()), QCoreApplication::instance(), SLOT(quit()));


    if(shellMode) {
        rescanAvailableFunctions();


        new CursesAction("Add User", &_system);
        new CursesAction("Add Group", &_system);
        action = new CursesAction("Edit Cron Tab", &_system);
        connect(action, SIGNAL(activated()), this, SLOT(editCronTab()));

        _system.addSeparator();

        action = new CursesAction("Install _Updates", &_system);
        connect(action, SIGNAL(activated()), this, SLOT(aptUpdateUpgrade()));
        action = new CursesAction("Agressive Upgrade", &_system);
        connect(action, SIGNAL(activated()), this, SLOT(aptUpdateDistUpgrade()));

        _system.addSeparator();

        action = new CursesAction("Restart", &_system);
        connect(action, SIGNAL(activated()), this, SLOT(sudoReboot()));

        connect(&_rescanTimer, SIGNAL(timeout()), this, SLOT(rescanAvailableFunctions()));
        _rescanTimer.setInterval(2000);
        _rescanTimer.start();
    }


    new CursesAction("Contents", &_help);
    new CursesAction("Credits", &_help);
    new CursesAction("Website", &_help);
    new CursesAction("Source", &_help);
    new CursesAction("Donate", &_help);


    _coordinator.action()->setParent(&_menuBar);
    if(shellMode) {
        _launch.action()->setParent(&_menuBar);
        _screens.action()->setParent(&_menuBar);
        _system.action()->setParent(&_menuBar);
    }
    _help.action()->setParent(&_menuBar);


    connect(&launchVim, SIGNAL(activated()), this, SLOT(runVim()));
    connect(&launchNano, SIGNAL(activated()), this, SLOT(runNano()));

    connect(&launchLynx, SIGNAL(activated()), this, SLOT(runLynx()));
    connect(&launchELinks, SIGNAL(activated()), this, SLOT(runELinks()));
    connect(&launchW3M, SIGNAL(activated()), this, SLOT(runW3M()));

    connect(&installScreen, SIGNAL(activated()), this, SLOT(installScreenPkg()));


    updateStatusMessage();
    fixLayoutImpl();
    child_pid = 0;
}

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

    // Rescan Screens
    typedef QSharedPointer<ScreenAction> Screen;
    static QHash<QString, Screen> screens;

    static QDir screenDir(SCREEN_DIR);
    static char* user = getenv("USER");
    if(screenDir.exists()) {
        screenListSeparator.show();
        installScreen.hide();
        createScreen.show();

        if(user) {
            QDir userDir(QString(SCREEN_DIR "/S-%1").arg(user));
            if(userDir.exists()) {
                QHash<QString, Screen> oldScreens = screens;
                screens.clear();

                static QRegExp nameFormat("(\\d+)\\.(.+)");
                foreach(QFileInfo info, userDir.entryInfoList(QDir::Files | QDir::System | QDir::Hidden)) {
                    QString name = info.fileName();

                    if(!nameFormat.exactMatch(name))
                        continue;

                    bool idOkay;
                    int id = nameFormat.cap(1).toInt(&idOkay);
                    if(!idOkay)
                        continue;

                    name = nameFormat.cap(2);
                    Screen scr = oldScreens.value(name);
                    if(!scr)
                        scr = Screen(new ScreenAction(id, name, &_screens));
                    screens.insert(scr->text(), scr);
                }

                screenNoInstancesMessage.setVisible(screens.isEmpty());
            } else {
                screenNoInstancesMessage.show();
                screens.clear();
            }
        } else {
            screenNoInstancesMessage.show();
            screens.clear();
        }


        screenDir.entryList();
    } else {
        screenNoInstancesMessage.hide();
        screenListSeparator.hide();
        installScreen.show();
        createScreen.hide();

        screens.clear();
    }
}

bool ScreenAction::processEvent(QEvent *e) {
    if(e->type() == GUIEvent::GUIActivated) {
        CoordinatorConsole* con = (CoordinatorConsole*)CursesMainWindow::current();
        if(con) {
            con->startShell(QStringList() << "screen" << "-x" << QString("%1.%2").arg(_id).arg(name()));
            return true;
        }
    }
    return CursesAction::processEvent(e);
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
        _statusQueue << QString("`%1` finished").arg(quotedCmd);
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
    startShell(QStringList() << "sudo" << "apt-get" << "install" << pkg, "Follow the instructions below, they will help you install the required software.\n\n");
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

