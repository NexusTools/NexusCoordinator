#ifndef NCURSESMAIN_H
#define NCURSESMAIN_H

#include <QStringList>
#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QDebug>

#include "curses.h"

//

class NCursesMain : public QObject, NCursesWin {
    Q_OBJECT

public:
    enum FocusTarget {
        MenuFocusTarget,
        LogFocusTarget,
        ScopeFocusTarget
    };

    NCursesMain(QString title ="") : NCursesWin(initscr()) {
        start_color();

        nodelay(hwnd(), true);
        keypad(hwnd(), true);
        meta(hwnd(), true);

        //nocbreak();
        //noecho();
        //raw();

        menuFocusID = 0;
        focusTarget = MenuFocusTarget;

        refreshTimer.setInterval(0);
        refreshTimer.setSingleShot(true);
        connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(drawNow()));
        refreshTimer.start();

        setTitle(title);
        checkSize();
    }

    inline void checkSize() {
        endwin();
        refresh();

        updateSize(QSize(getmaxx(hwnd()), getmaxy(hwnd())));
    }

    inline void setTitle(QString) {}

protected:
    void initImpl() {refresh();}

    void geometryChanged(QRect) {}
    void positionChanged(QPoint) {}
    void sizeChanged(QSize) {
        draw();
    }

    void refresh() {
        refreshTimer.start();
    }

    void draw() {
        move(0, 0);
        int id = 0;
        bool focusMenu = false;
        bool focusRect = focusTarget == MenuFocusTarget;
        foreach(QString menu, menuBar) {
            if(focusRect && (focusMenu = (menuFocusID == id)))
                attrset(A_STANDOUT);
            addch(' ');
            foreach(char c, menu.toLocal8Bit()) {
                if(c == '&')
                    attron(A_UNDERLINE);
                else {
                    addch(c);
                    attroff(A_UNDERLINE);
                }
            }
            addch(' ');
            if(focusMenu) {
                attrset(A_NORMAL);
                focusRect = false;
            }
            id++;
        }

        // Date Time
        QString time = QDateTime::currentDateTime().toString();
        writeString(QPoint(width()-time.length(), 0), time);

        // General Status

        // divider
        move(1, 0);
        int l = width();
        while(l-- > 0)
            addch(l == 20 ? ACS_TTEE : ACS_HLINE);

        // process children

        doupdate();
    }

public slots:
    void drawNow() {draw();}

    inline void readNextCH() {
        int ch = getch();

        if(ch < 0 || ch > KEY_MAX)
            return;

        if(ch == KEY_RESIZE)
            checkSize();
    }

private:
    QStringList menuBar;
    FocusTarget focusTarget;
    int menuFocusID;

    QList<NCursesWin*> _children;
    QTimer refreshTimer;
    QTimer keyTimer;
};

#endif // NCURSESMAIN_H
