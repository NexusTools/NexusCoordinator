#ifndef COORDINATORCONSOLE_H
#define COORDINATORCONSOLE_H

#include <cursesmainwindow.h>
#include <cursesmenuseparator.h>
#include <cursesmenubar.h>
#include <cursesmenu.h>

#include <QCoreApplication>
#include <QStringList>
#include <QDateTime>
#include <QFileInfo>
#include <QSettings>
#include <QTimer>
#include <QTime>
#include <QFile>

#include "updatedialog.h"

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

    friend class CoordinatorUpdateDialog;
public:
    explicit CoordinatorConsole(bool shellMode);

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

    static QString quoteArg(QString arg);

public slots:
    inline void notifyClicked() {
        _statusQueue << "I was clicked!";
    }

    inline void blinkStatus() {
        _statusBar.setAttr(_statusBar.attr() ^ CursesLabel::Standout);
    }

    bool startShell(QStringList, QByteArray startMsg ="", QByteArray title="", QString finMsg ="", QString workingDir ="");
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
            flash();
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
    void killChild();

protected:
    inline virtual void fixLayoutImpl() {
        CursesMainWindow::fixLayoutImpl();
        _statusBar.move(width()-_statusBar.width(), 0);
    }

protected slots:
    void configure();
    void setTheme(QString name);

    inline void installScreenPkg() {
        aptInstall("screen");
    }
    void aptInstall(QString package);
    void aptUpdateUpgrade();
    void aptUpdateDistUpgrade();


    void sudoReboot();
    void dropToShell();
    void dropToRootShell();
    void editCronTab();
    void sigIntDiag();


    void runVim(QString file =QString());
    void runNano(QString file =QString());

    void runELinks(QString url =QString());
    void runLynx(QString url =QString());
    void runW3M(QString url =QString());


    void createScreen();


    void createUser();
    void rescanAvailableFunctions();

private:
    int child_pid;
    bool _shellMode;
    bool _upgraded;

    QTimer _blinkTimer;
    QTimer _rescanTimer;
    QTimer _updateDateTime;
    QStringList _statusQueue;
    CoordinatorUpdateDialog _updateDiag;
    QSettings _config;


    CursesMenuBar _menuBar;

    CursesMenu _coordinator;
    CursesMenu _launch;
    CursesMenu _screens;
    CursesMenu _system;
    CursesMenu _help;


    CursesAction launchVim;
    CursesAction launchNano;

    CursesAction launchW3M;
    CursesAction launchELinks;
    CursesAction launchLynx;


    CursesAction _installScreen;
    CursesAction _createScreen;
    CursesAction manageOtherUser;
    CursesMenuSeparator screenListSeparator;
    CursesLabel screenNoInstancesMessage;


    CursesLabel _statusBar;
};

#endif // COORDINATORCONSOLE_H
