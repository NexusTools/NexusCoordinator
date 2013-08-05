#ifndef COORDINATORCONSOLE_H
#define COORDINATORCONSOLE_H

#include <cursesmainwindow.h>
#include <cursesmenuseparator.h>
#include <cursesmenubar.h>
#include <cursesmenu.h>

#include <QCoreApplication>
#include <QStringList>
#include <QDateTime>
#include <QTimer>
#include <QTime>
#include <QFile>

inline QString readHostname() {
    QFile f("/etc/hostname");
    if(f.open(QFile::ReadOnly))
        return QString::fromUtf8(f.readAll());
    return "Unknown";
}

inline QStringList getLoginMessages() {
    QStringList messages;
    messages << QString("Logged in as %1").arg(getenv("USER"));
    char* clientStr = getenv("SSH_CLIENT");
    if(clientStr) {
        QString client = QString::fromLocal8Bit(clientStr);
        int pos = client.indexOf(' ');
        messages << QString("via %1").arg(client.mid(0, pos));
    }

    return messages;
}

class CoordinatorConsole : public CursesMainWindow
{
    Q_OBJECT
public:
    inline explicit CoordinatorConsole() : CursesMainWindow(QString("NexusCoordinator on %1").arg(readHostname())), _menuBar(this), _coordinator("Coord_inator", this), _screens("Scree_ns", this), _help("_Help", this),
        createScreen("Create Screen", &_screens), installScreen("Install Screen", &_screens), screenListSeparator(&_screens), _statusBar(this) {
        _updateDateTime.setInterval(1000);
        connect(&_updateDateTime, SIGNAL(timeout()), this, SLOT(updateStatusMessage()));
        _updateDateTime.start();

        _blinkTimer.setInterval(600);
        connect(&_blinkTimer, SIGNAL(timeout()), this, SLOT(blinkStatus()));
        connect(&_statusBar, SIGNAL(clicked()), this, SLOT(notifyClicked()));

        CursesAction* action = new CursesAction("_Drop to Shell", &_coordinator);
        connect(action, SIGNAL(activated()), this, SLOT(dropToShell()), Qt::QueuedConnection);
        action = new CursesAction("Drop to _Root Shell", &_coordinator);
        connect(action, SIGNAL(activated()), this, SLOT(dropToRootShell()), Qt::QueuedConnection);

        _coordinator.addSeparator();

        action = new CursesAction("Connect to...", &_coordinator);
        action->disable();
        action = new CursesAction("Disconnect", &_coordinator);
        action->disable();

        _coordinator.addSeparator();

        action = new CursesAction("E_xit", &_coordinator);
        connect(action, SIGNAL(activated()), QCoreApplication::instance(), SLOT(quit()));

        new CursesAction("Contents", &_help);
        new CursesAction("Credits", &_help);
        new CursesAction("Website", &_help);
        new CursesAction("Source", &_help);
        new CursesAction("Donate", &_help);

        _coordinator.action()->setParent(&_menuBar);
        _screens.action()->setParent(&_menuBar);
        _help.action()->setParent(&_menuBar);

        connect(&installScreen, SIGNAL(activated()), this, SLOT(installScreenPkg()));

        rescanScreens();
        updateStatusMessage();
        fixLayoutImpl();
        fork_rv = 0;
    }

    inline void drawImpl() {
        wmove(hnd(), 1, 0);
        int rem = width();
        while(rem > 0) {
            waddch(hnd(), rem == 20 ? ACS_TTEE : ACS_HLINE);
            rem--;
        }
        wmove(hnd(), height()-6, 0);
        rem = width()-20;
        while(rem > 0) {
            waddch(hnd(), ACS_HLINE);
            rem--;
        }
        int y = 2;
        int x = width()-20;
        rem = height()-2;
        while(rem > 0) {
            wmove(hnd(), y, x);
            waddch(hnd(), rem == 6 ? ACS_RTEE : ACS_VLINE);
            rem--;
            y++;
        }
    }

public slots:
    inline void notifyClicked() {
        _statusQueue << "I was clicked!";
    }

    inline void blinkStatus() {
        _statusBar.setAttr(_statusBar.attr() ^ CursesLabel::Standout);
    }

    inline void updateStatusMessage() {
        QDateTime dateTime = QDateTime::currentDateTime();
        QString nextMessage;

        static QStringList loginMessages(getLoginMessages());

        int timeout = 1500;
        _blinkTimer.stop();
        _statusBar.setAttr(CursesLabel::Dim);
        if(!loginMessages.isEmpty()) {
            nextMessage = loginMessages.takeFirst();
            timeout += 1500;
        } else if(!_statusQueue.isEmpty()) {
            timeout += 1500;
            _blinkTimer.start();
            nextMessage = _statusQueue.takeFirst();
            _statusBar.setAttr(CursesLabel::Attr(CursesLabel::Bold | CursesLabel::Standout));
        } else if(dateTime.time().second() % 15 == 0) {
            if(dateTime.time().second() == 0)
                nextMessage = QString("%1 V%2").arg(QCoreApplication::instance()->applicationName()).arg(QCoreApplication::instance()->applicationVersion());
            else {
                QFile f("/proc/loadavg");
                if(f.open(QFile::ReadOnly)) {
                    QString loadAvg = QString::fromLocal8Bit(f.readAll());

                    int index = loadAvg.indexOf(' ');
                    index = loadAvg.indexOf(' ', index+1);
                    index = loadAvg.indexOf(' ', index+1);

                    nextMessage = QString("Load: %1").arg(loadAvg.mid(0, index));
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

    virtual void terminateRequested(int);

protected:
    inline virtual void fixLayoutImpl() {
        CursesMainWindow::fixLayoutImpl();

        _menuBar.resize(QSize(80, 1));
        _statusBar.move(width()-_statusBar.width(), 0);
    }

protected slots:
    inline void installScreenPkg() {
        aptInstall("screen");
    }
    void aptInstall(QString package);

    void dropToShell();
    void dropToRootShell();

    void rescanScreens();

private:
    int fork_rv;
    bool _terminated;

    QTimer _blinkTimer;
    QTimer _updateDateTime;
    QStringList _statusQueue;

    CursesMenuBar _menuBar;
    CursesMenu _coordinator;
    CursesMenu _screens;
    CursesMenu _help;

    CursesAction createScreen;
    CursesAction installScreen;
    CursesMenuSeparator screenListSeparator;
    QHash<QString, CursesAction*> _screenList;

    CursesLabel _statusBar;
};

#endif // COORDINATORCONSOLE_H
