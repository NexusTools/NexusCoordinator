#ifndef COORDINATORCONSOLE_H
#define COORDINATORCONSOLE_H

#include <cursesmainwindow.h>
#include <curseslabel.h>

#include <QCoreApplication>
#include <QStringList>
#include <QDateTime>
#include <QTimer>
#include <QTime>

class CoordinatorConsole : public CursesMainWindow
{
    Q_OBJECT
public:
    inline explicit CoordinatorConsole() : CursesMainWindow("NexusCoordinator Console"), _dateTime(this) {
        _updateDateTime.setInterval(1000);
        connect(&_updateDateTime, SIGNAL(timeout()), this, SLOT(updateDateTime()));
        _updateDateTime.start();

        _blinkTimer.setInterval(400);
        connect(&_blinkTimer, SIGNAL(timeout()), this, SLOT(blinkDateTime()));

        connect(&_dateTime, SIGNAL(clicked()), this, SLOT(notifyClicked()));

        QTimer::singleShot(1500, this, SLOT(testMessageSystem()));
        updateDateTime();
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
        _messages << "I was clicked!";
    }

    inline void testMessageSystem() {
        _messages << "Tuna Fish!";
    }

    inline void blinkDateTime() {
        _dateTime.setAttr(_dateTime.attr() ^ CursesLabel::Standout);
    }

    inline void updateDateTime() {
        QDateTime dateTime = QDateTime::currentDateTime();
        QString nextMessage;

        _blinkTimer.stop();
        _dateTime.setAttr(CursesLabel::Dim);
        if(!_messages.isEmpty()) {
            _blinkTimer.start();
            nextMessage = _messages.takeFirst();
            _dateTime.setAttr(CursesLabel::Attr(CursesLabel::Bold));
        } else if(dateTime.time().second() % 30 == 0)
            nextMessage = QString("%1 V%2").arg(QCoreApplication::instance()->applicationName()).arg(QCoreApplication::instance()->applicationVersion());

        if(!nextMessage.isEmpty()) {
            _updateDateTime.stop(); // Skip 1.5 seconds, wait 1 more
            QTimer::singleShot(1500, &_updateDateTime, SLOT(start()));

            _dateTime.setText(nextMessage);
        } else
            _dateTime.setText(QDateTime::currentDateTime().toString());

        _dateTime.move(width()-_dateTime.width(), 0);
    }

protected:
    inline virtual void fixLayoutImpl() {
        CursesMainWindow::fixLayoutImpl();
        _dateTime.move(width()-_dateTime.width(), 0);
    }

private:
    QTimer _blinkTimer;
    QTimer _updateDateTime;
    CursesLabel _dateTime;
    QStringList _messages;
};

#endif // COORDINATORCONSOLE_H
