#ifndef COORDINATORCONSOLE_H
#define COORDINATORCONSOLE_H

#include <cursesmainwindow.h>
#include <cursesmenubar.h>
#include <curseslabel.h>
#include <cursesaction.h>

#include <QCoreApplication>
#include <QStringList>
#include <QDateTime>
#include <QTimer>
#include <QTime>

class CoordinatorConsole : public CursesMainWindow
{
    Q_OBJECT
public:
    inline explicit CoordinatorConsole() : CursesMainWindow("NexusCoordinator Console"), _menuBar(this), _statusBar(this) {
        _updateDateTime.setInterval(1000);
        connect(&_updateDateTime, SIGNAL(timeout()), this, SLOT(updateStatusMessage()));
        _updateDateTime.start();

        _blinkTimer.setInterval(600);
        connect(&_blinkTimer, SIGNAL(timeout()), this, SLOT(blinkStatus()));
        connect(&_statusBar, SIGNAL(clicked()), this, SLOT(notifyClicked()));

        QStringList menus;
        menus << "Servers" << "Theme" << "Help" << "Quit";
        foreach(QString menu, menus)
            new CursesAction(menu, &_menuBar);

        updateStatusMessage();
        fixLayoutImpl();
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

        int timeout = 1500;
        _blinkTimer.stop();
        _statusBar.setAttr(CursesLabel::Dim);
        if(!_statusQueue.isEmpty()) {
            timeout += 1500;
            _blinkTimer.start();
            nextMessage = _statusQueue.takeFirst();
            _statusBar.setAttr(CursesLabel::Attr(CursesLabel::Bold | CursesLabel::Standout));
        } else if(dateTime.time().second() % 30 == 0)
            nextMessage = QString("%1 V%2").arg(QCoreApplication::instance()->applicationName()).arg(QCoreApplication::instance()->applicationVersion());

        if(!nextMessage.isEmpty()) {
            _updateDateTime.stop(); // Skip 1.5 seconds, wait 1 more
            QTimer::singleShot(timeout, &_updateDateTime, SLOT(start()));

            _statusBar.setText(nextMessage);
        } else
            _statusBar.setText(QDateTime::currentDateTime().toString());

        fixLayoutImpl();
    }

protected:
    inline virtual void fixLayoutImpl() {
        CursesMainWindow::fixLayoutImpl();

        _menuBar.resize(width()-_statusBar.width(), 1);
        _statusBar.move(width()-_statusBar.width(), 0);
    }

private:
    QTimer _blinkTimer;
    QTimer _updateDateTime;
    QStringList _statusQueue;

    CursesMenuBar _menuBar;
    CursesLabel _statusBar;
};

#endif // COORDINATORCONSOLE_H
