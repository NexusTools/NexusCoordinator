#include "coordinatorconsole.h"
#include "screenaction.h"

#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

#include <curseslineedit.h>
#include <cursesdialog.h>

#include <QCoreApplication>
#include <QDir>

#define SCREEN_DIR "/var/run/screen"

inline QString readHostname() {
    QFile f("/etc/hostname");
    if(f.open(QFile::ReadOnly)) {
        QString hostname = QString::fromUtf8(f.readAll()).trimmed();
        if(!hostname.isEmpty()) {
            f.close();
            f.setFileName("/etc/hostgroup");
            if(f.open(QFile::ReadOnly)) {
                QString hostgroup = QString::fromUtf8(f.readAll()).trimmed();
                if(!hostgroup.isEmpty()) {
                    hostname += '[';
                    hostname += hostgroup;
                    hostname += ']';
                }
            }

            return hostname;
        }
    }
    return "Unknown";
}

void keyboardSignal(int s) {
    ((CoordinatorConsole*)CoordinatorConsole::current())->terminateRequested(s);
}

CoordinatorConsole::CoordinatorConsole(bool shellMode) : CursesMainWindow(
            shellMode ? QString("NexusCoordinator Shell on %1").arg(readHostname()) : QString("NexusCoordinator V%1").arg(QCoreApplication::instance()->applicationVersion())),
            _updateDiag(this), _menuBar(this), _coordinator("Coord_inator", this), _launch("_Launch", this), _screens("Scree_ns", this), _system("S_ystem", this), _help("_Help", this),
            launchVim("Vim", &_launch), launchNano("Nano", &_launch), launchW3M("W3M", &_launch), launchELinks("ELinks", &_launch), launchLynx("Lynx", &_launch),
            _installScreen("Install Screen", &_screens), _createScreen("Create Screen", &_screens), manageOtherUser("Other Screens...", &_screens), screenListSeparator(&_screens), screenNoInstancesMessage(" No Active Screens ", &_screens), _statusBar(this) {

    _upgraded = false;
    if(shellMode)
        signal(SIGTSTP, keyboardSignal);

    _shellMode = shellMode;
    _updateDateTime.setInterval(1000);
    connect(&_updateDateTime, SIGNAL(timeout()), this, SLOT(updateStatusMessage()));
    _updateDateTime.start();

    _blinkTimer.setInterval(600);
    connect(&_blinkTimer, SIGNAL(timeout()), this, SLOT(blinkStatus()));
    connect(&_statusBar, SIGNAL(clicked()), this, SLOT(notifyClicked()));

    QString theme = _config.value("theme").toString();
    setTheme(theme.isEmpty() ? (getuid() == 0 ? "Red" : "Default") : theme);

    CursesAction* action;
    if(shellMode) {
        action = new CursesAction("_Drop to Shell", &_coordinator);
        connect(action, SIGNAL(activated()), this, SLOT(dropToShell()), Qt::QueuedConnection);
        if(getuid() != 0) {
            action = new CursesAction("Drop to _Root Shell", &_coordinator);
            connect(action, SIGNAL(activated()), this, SLOT(dropToRootShell()), Qt::QueuedConnection);

            _coordinator.addSeparator();
        }

        action = new CursesAction("Run as User...", &_coordinator);
        connect(action, SIGNAL(activated()), this, SLOT(runAsUser()), Qt::QueuedConnection);

        if(getuid() != 0) {
            action = new CursesAction("Run as Root", &_coordinator);
            connect(action, SIGNAL(activated()), this, SLOT(runAsRoot()), Qt::QueuedConnection);
        }
    } else {
        action = new CursesAction("Connect to...", &_coordinator);
        action->disable();
        action = new CursesAction("Disconnect", &_coordinator);
        action->disable();
    }

    _coordinator.addSeparator();

    action = new CursesAction("Con_figure", &_coordinator);
    connect(action, SIGNAL(activated()), this, SLOT(configure()));
    action = new CursesAction("U_pdate", &_coordinator);
    connect(action, SIGNAL(activated()), &_updateDiag, SLOT(show()));

    _coordinator.addSeparator();

    action = new CursesAction(_shellMode ? QString("Lo_g out %1").arg(getenv("USER")) : "E_xit", &_coordinator);
    connect(action, SIGNAL(activated()), QCoreApplication::instance(), SLOT(quit()));


    if(shellMode) {
        rescanAvailableFunctions();
        connect(&_createScreen, SIGNAL(activated()), this, SLOT(createScreen()));

        action = new CursesAction("Create User", &_system);
        connect(action, SIGNAL(activated()), this, SLOT(createUser()));
        //new CursesAction("Add Group", &_system);
        action = new CursesAction("Edit Cron Tab", &_system);
        connect(action, SIGNAL(activated()), this, SLOT(editCronTab()));

        _system.addSeparator();

        action = new CursesAction("Install _Updates", &_system);
        connect(action, SIGNAL(activated()), this, SLOT(aptUpdateUpgrade()));
        action = new CursesAction("Agressive Upgrade", &_system);
        connect(action, SIGNAL(activated()), this, SLOT(aptUpdateDistUpgrade()));

        _system.addSeparator();

        action = new CursesAction("Re_boot", &_system);
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

    connect(&_installScreen, SIGNAL(activated()), this, SLOT(installScreenPkg()));


    updateStatusMessage();
    fixLayoutImpl();
    child_pid = 0;

    if(getuid() == 0)
        QTimer::singleShot(5, this, SLOT(rootMessage()));
    else
        QTimer::singleShot(5, this, SLOT(checkUpdated()));
}

void killChildAtExit() {
    ((CoordinatorConsole*)CursesMainWindow::current())->killChild();
}

void CoordinatorConsole::killChild() {
    if(child_pid > 0) {
        kill(child_pid, SIGTERM);
        kill(child_pid, SIGKILL);
    }
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
        manageOtherUser.show();
        _installScreen.hide();
        _createScreen.show();

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
        manageOtherUser.hide();
        _installScreen.show();
        _createScreen.hide();

        screens.clear();
    }
}

bool ScreenAction::processEvent(QEvent *e) {
    if(e->type() == GUIEvent::GUIActivated) {
        CoordinatorConsole* con = (CoordinatorConsole*)CursesMainWindow::current();
        if(con) {
            con->startShell(QStringList() << "screen" << "-x" << QString("%1.%2").arg(_id).arg(name()), "", QString("%1 ~ Screen").arg(name()).toLocal8Bit());
            return true;
        }
    }
    return CursesAction::processEvent(e);
}

void CoordinatorConsole::sigIntDiag() {
    static bool tryReboot = false;
    if(_shellMode && !tryReboot) {
        tryReboot = true;
        QString option = CursesDialog::options(QStringList() << "E_xit" << "_Always Exit" << "_Drop to Shell" << "Cl_ose", "You pressed CTRL+C, or something generated a INT signal.", "Signal INT Captured");
        if(option == "Drop to Shell")
            dropToShell();

        tryReboot = false;
        if(!option.endsWith("Exit"))
            return;
    }

    CursesMainWindow::terminateRequested(SIGINT);
}

void CoordinatorConsole::createScreen() {
    CursesDialog* diag = new CursesDialog("Create Screen", this);
    connect(diag, SIGNAL(finished()), diag, SLOT(deleteLater()));
    diag->setLayout(GUIContainer::VerticalLayout);

    CursesVBox* msg = new CursesVBox(diag);
    new CursesLabel("This will create a new screen.", msg);


    CursesHBox* columns = new CursesHBox(Spacing(1, 0), diag);
    CursesVBox* cell = new CursesVBox(columns);
    cell->setWAttr(GUIWidget::FloatCenter);
    new CursesLabel("Name", cell);
    CursesLineEdit* name = new CursesLineEdit(cell);

    cell = new CursesVBox(columns);
    cell->setWAttr(GUIWidget::FloatCenter);
    new CursesLabel("Command", cell);
    CursesLineEdit* command = new CursesLineEdit("bash", cell);

//    columns = new CursesHBox(Spacing(1, 0), diag);
//    cell = new CursesVBox(columns);
//    cell->setWAttr(GUIWidget::FloatCenter);
//    new CursesLabel("Shared", cell);
//    new CursesLineEdit(cell);

//    cell = new CursesVBox(columns);
//    cell->setWAttr(GUIWidget::FloatCenter);
//    new CursesLabel("As User", cell);
//    new CursesLineEdit(getenv("USER"), cell);



    CursesButtonBox* buttonContainer = new CursesButtonBox(diag);
    foreach(QString option, QStringList() << "Cre_ate Screen" << "_Nevermind" << "_Help") {
        CursesButton* act = new CursesButton(option, GUIWidget::FloatCenter, buttonContainer);
        connect(act, SIGNAL(selected(QVariant)), diag, SLOT(answer(QVariant)));
    }

    forever {
        diag->exec();

        QString ret = diag->value<QString>();
        if(ret == "Create Screen")
            startShell(QStringList() << "screen" << "-S" << name->text() << command->text(), "Creating screen ...", QString("%1 ~ Shell").arg(name->text()).toLocal8Bit());
        else if(ret == "Help")
            continue;
        break;
    }
}

void CoordinatorConsole::createUser() {
    CursesDialog* diag = new CursesDialog("Create User", this);
    connect(diag, SIGNAL(finished()), diag, SLOT(deleteLater()));
    diag->setLayout(GUIContainer::VerticalLayout);

    CursesVBox* msg = new CursesVBox(diag);
    new CursesLabel("This will create a new system user with", msg);
    new CursesLabel("access to ssh, ftp and mail.", msg);

    msg = new CursesVBox(diag);
    new CursesLabel("This does not effect the database of any", msg);
    new CursesLabel("websites or other services.", msg);

    CursesHBox* columns = new CursesHBox(Spacing(1, 0), diag);
    CursesVBox* cell = new CursesVBox(columns);
    cell->setWAttr(GUIWidget::FloatCenter);
    new CursesLabel("Username", cell);
    CursesLineEdit* user = new CursesLineEdit(cell);

    cell = new CursesVBox(columns);
    cell->setWAttr(GUIWidget::FloatCenter);
    new CursesLabel("Shell", cell);
    CursesLineEdit* shell = new CursesLineEdit("/bin/nc-shell", cell);

    CursesButtonBox* buttonContainer = new CursesButtonBox(diag);
    foreach(QString option, QStringList() << "Cre_ate User" << "_Nevermind") {
        CursesButton* act = new CursesButton(option, GUIWidget::FloatCenter, buttonContainer);
        connect(act, SIGNAL(selected(QVariant)), diag, SLOT(answer(QVariant)));
    }

    diag->exec();
    if(diag->value<QString>() == "Create User")
        startShell(QStringList() << "sudo" << "adduser" << "--shell" << shell->text() << user->text());
}

void CoordinatorConsole::checkUpdated() {
    QString ver = _config.value("version").toString();
    QString cver = QCoreApplication::instance()->applicationVersion();
    if(!ver.isEmpty() && QCoreApplication::instance()->applicationVersion() != ver) {
        _config.setValue("version", QCoreApplication::instance()->applicationVersion());
        CursesDialog* diag = new CursesDialog("Coordinator Updated", this);
        diag->setLayout(GUIContainer::VerticalLayout);

        new CursesLabel("NexusCoordinator has been updated from", diag);
        new CursesLabel(QString("version %1 to %2!").arg(ver).arg(cver), diag);

        CursesButtonBox* btnBox = new CursesButtonBox(diag);
        new CursesButton("O_kay", GUIWidget::Normal, btnBox);
        new CursesButton("Chan_gelog", GUIWidget::Normal, btnBox);

        diag->exec();
    }

    _config.setValue("version", QCoreApplication::instance()->applicationVersion());
}

void CoordinatorConsole::rootMessage() {
    if(_shellMode) {
        if(CursesDialog::options(QStringList() << "E_xit" << "Continu_e", "The root user has unrestricted access, continue?", "Root User") == "Continue")
            return;
    } else
        CursesDialog::alert("This software cannot run as root.", "Root User");

    endwin();
    exit(1);
}

void CoordinatorConsole::configure() {
    CursesDialog* diag = new CursesDialog("Configure NexusCoordinator", this);
    connect(diag, SIGNAL(finished()), diag, SLOT(deleteLater()));
    diag->setLayout(GUIContainer::VerticalLayout);

    CursesVBox* vBox = new CursesVBox(diag);
    new CursesLabel("Currently only the theme is configurable,", vBox);
    new CursesLabel("Click a theme below to change to it.", vBox);

    CursesHBox* hBox = new CursesHBox(Spacing(1, 0), diag);
    foreach(QString option, QStringList() << "_Default" << "_Green" << "_Magenta") {
        CursesButton* act = new CursesButton(option, GUIWidget::FloatCenter, hBox);
        connect(act, SIGNAL(selected(QVariant)), diag, SLOT(answer(QVariant)));
    }
    hBox = new CursesHBox(Spacing(1, 0), diag);
    foreach(QString option, QStringList() << "C_yan" << "_Blue" << "_Red" << "_Yellow"){
        CursesButton* act = new CursesButton(option, GUIWidget::FloatCenter, hBox);
        connect(act, SIGNAL(selected(QVariant)), diag, SLOT(answer(QVariant)));
    }

    CursesButtonBox* buttonContainer = new CursesButtonBox(diag);
    CursesButton* act = new CursesButton("Clos_e", GUIWidget::FloatRight, buttonContainer);
    connect(act, SIGNAL(selected(QVariant)), diag, SLOT(answer(QVariant)));

    forever {
        diag->exec();
        QString val = diag->value<QString>();
        if(val == "Close")
            break;

        setTheme(val);
        continue;
    }
}

void CoordinatorConsole::setTheme(QString name) {
    if(name == "Default") {
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_CYAN, COLOR_BLACK);
    } else if(name == "Green") {
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
    } else if(name == "Magenta") {
        init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
    } else if(name == "Cyan") {
        init_pair(1, COLOR_CYAN, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
    } else if(name == "Red") {
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
    } else if(name == "Blue") {
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
    } else if(name == "Yellow") {
        init_pair(1, COLOR_YELLOW, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
    } else
        return;

    _config.setValue("theme", name);
}

void CoordinatorConsole::updateStatusMessage() {
    QDateTime dateTime = QDateTime::currentDateTime();
    QString nextMessage;

    static QStringList subtleMessageQueue(getLoginMessages());

    int timeout = 1500;
    _blinkTimer.stop();
    _statusBar.setAttr(CursesLabel::Dim);
    if(!subtleMessageQueue.isEmpty()) {
        if(subtleMessageQueue.size() > 1)
            timeout += 1500;
        nextMessage = subtleMessageQueue.takeFirst();
    } else if(!_statusQueue.isEmpty()) {
        flash();
        timeout += 1500;
        _blinkTimer.start();
        nextMessage = _statusQueue.takeFirst();
        _statusBar.setAttr(CursesLabel::Attr(CursesLabel::Bold | CursesLabel::Standout));
    } else if(dateTime.time().second() % 15 == 0) {
        switch(dateTime.time().second()) {
            case 0:
                nextMessage = QString("%1 V%2").arg(QCoreApplication::instance()->applicationName()).arg(QCoreApplication::instance()->applicationVersion());
                subtleMessageQueue << QString("On %1").arg(readHostname());
                break;

            case 15:
            {
                QFile f("/proc/loadavg");
                if(f.open(QFile::ReadOnly)) {
                    QString loadAvg = QString::fromLocal8Bit(f.readAll());

                    int index = loadAvg.indexOf(' ');
                    index = loadAvg.indexOf(' ', index+1);
                    index = loadAvg.indexOf(' ', index+1);

                    nextMessage = QString("Load: %1").arg(loadAvg.mid(0, index));
                }
                break;
            }

            case 30:
            {
                QFile f("/proc/uptime");
                if(f.open(QFile::ReadOnly)) {
                    QString loadAvg = QString::fromLocal8Bit(f.readAll());

                    int index = loadAvg.indexOf(' ');
                    int elapsed = loadAvg.mid(0, index).toInt();


                    nextMessage = QString("Uptime: %1").arg(elapsed);
                }
                break;
            }

            case 45:
            {
                QFile f("/proc/meminfo");
                if(f.open(QFile::ReadOnly)) {
                    QString loadAvg = QString::fromLocal8Bit(f.readAll());

                    int index = loadAvg.indexOf(' ');
                    index = loadAvg.indexOf(' ', index+1);
                    index = loadAvg.indexOf(' ', index+1);

                    nextMessage = QString("Memory: %1").arg(loadAvg.mid(0, index));
                }
                break;
            }
        }
    }

    if(!nextMessage.isEmpty()) {
        _updateDateTime.stop(); // Skip 1.5 seconds, wait 1 more
        QTimer::singleShot(timeout, &_updateDateTime, SLOT(start()));

        _statusBar.setText(nextMessage);
    } else
        _statusBar.setText(QDateTime::currentDateTime().toString());
}

void CoordinatorConsole::terminateRequested(int sig) {
    if(sig == SIGTSTP)
        kill(getpid(), SIGCONT);
    else if(child_pid > 0) {
        _terminated = true;
        kill(child_pid, sig);
    } else if(sig == SIGINT)
        metaObject()->invokeMethod(this, "sigIntDiag", Qt::QueuedConnection);
    else
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

bool CoordinatorConsole::startShell(QStringList args, QByteArray startMsg, QByteArray title, QString finMsg, QString workingDir) {
    QString quotedCmd;
    QByteArray binaryPath;
    QString binary = args.first();
    {
        QFileInfo binaryInfo(binary);
        if(!binaryInfo.exists())
            binaryInfo.setFile(QString("bin:%1").arg(binary));
        else {
            binary = binaryInfo.fileName();
            args.replace(0, binary);
        }
        if(!binaryInfo.exists()) {
            _statusQueue << QString("Unable to locate `%1`").arg(binary);
            beep();
            return false;
        }
        binaryPath = binaryInfo.filePath().toLocal8Bit();
    }
    foreach(QString arg, args) {
        if(!quotedCmd.isEmpty())
            quotedCmd += ' ';

        quotedCmd += quoteArg(arg);
    }

    child_pid = fork();
    if (child_pid == 0)
    {
        endwin();

        printf("\033]0;%s\007\e[H\e[2J", qPrintable(title.isEmpty() ? quotedCmd : title));
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

        if(!workingDir.isEmpty())
            chdir(workingDir.toLocal8Bit().data());

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
        _statusQueue << QString("Failed to fork `%1`").arg(quotedCmd);
        beep();
        return false;
    }

    endwin();
    sleep(2);

    int status;
    bool ret = true;
    while (child_pid > 0 && waitpid(child_pid, &status, WUNTRACED) != -1) {
        if(WIFSTOPPED(status)) {
            if(_upgraded)
                kill(child_pid, SIGCONT);
            else {
                QString resp = CursesDialog::options(QStringList() << "Con_tinue" << "_Kill", "You pressed CTRL+Z, or stopped the running process.", "Process Stopped");
                if(resp == "Continue") {
                    endwin();
                    kill(child_pid, SIGCONT);
                } else {
                    kill(child_pid, SIGKILL);
                    break;
                }
            }
        } else {
            status = WEXITSTATUS(status);
            break;
        }
    }
    if(_upgraded) {
        if(status != 0) {
            printf("\ncrashed");

            sleep(2);
            return false;
        }
        return true;
    }

    if(status != 0 || _terminated) {
        _statusQueue << QString("`%1` exited (%2)").arg(quotedCmd).arg(status);
        ret = false;
        beep();
    } else if(!finMsg.isEmpty())
        _statusQueue << finMsg;
    else
        _statusQueue << QString("`%1` exited (%2)").arg(quotedCmd).arg(status);
    child_pid=0;

    rescanAvailableFunctions();
    titleChanged();
    refresh();

    return ret;
}

void CoordinatorConsole::aptUpdateUpgrade() {
    startShell(QStringList() << "sudo" << "bash" << "-c" << "apt-get update; apt-get upgrade", "Follow the instructions below.\n\n");
}

void CoordinatorConsole::aptUpdateDistUpgrade() {
    if(CursesDialog::ensure("This could be dangerous, are you sure?", "Aggressive Upgrade", &_config))
        startShell(QStringList() << "sudo" << "bash" << "-c" << "apt-get update; apt-get dist-upgrade", "Follow the instructions below.\n\n");
}

void CoordinatorConsole::aptInstall(QString pkg) {
    startShell(QStringList() << "sudo" << "apt-get" << "install" << pkg, "Follow the instructions below, they will help you install the required software.\n\n");
}

void CoordinatorConsole::runAsUser() {
    QString user = CursesDialog::input("Enter the name of the user", "Run As User...").trimmed();
    if(!user.isEmpty())
        startShell(QStringList() << "sudo" << "su" << "-s" << "/bin/nc-shell" << user);
}

void CoordinatorConsole::runAsRoot() {
    startShell(QStringList() << "sudo" << "su" << "-s" << "/bin/nc-shell");
}

void CoordinatorConsole::sudoReboot() {
    if(CursesDialog::ensure("This will reboot the server, are you sure?", "Reboot", &_config))
        startShell(QStringList() << "sudo" << "reboot", "Enter your password to reboot.\n\n");
}

void CoordinatorConsole::dropToShell() {
    startShell(QStringList() << "bash", "You have been dropped to a temporary shell.\nNexusCoordinator is still running, type 'exit' to return.\n\n", "Shell", "", getenv("HOME"));
}

void CoordinatorConsole::dropToRootShell() {
    if(CursesDialog::options(QStringList() << "Continu_e" << "Ca_ncel", "The root user has unrestricted access, continue?", "Root Shell", &_config) == "Continue")
        startShell(QStringList() << "sudo" << "bash", "You have been dropped to a temporary shell.\n\n", "Root Shell", "", "/");
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

