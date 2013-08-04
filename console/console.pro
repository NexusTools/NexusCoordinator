#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T10:08:53
#
#-------------------------------------------------

# Qt Modules
QT		 += xml
QT       -= gui

# Project Information
TARGET = NexusCoordinatorConsole
CONFIG   += console
CONFIG   -= app_bundle
LIBS += -lncurses

TEMPLATE = app

# Versioning
exists($$PWD/../extern/GitProjectVersionQt/version.pri) : include($$PWD/../extern/GitProjectVersionQt/version.pri)

# Project Files
SOURCES += main.cpp \
    coordinatorconsole.cpp

HEADERS += \
    coordinatorconsole.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../extern/GenericUI/core/release/ -lGenericUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../extern/GenericUI/core/debug/ -lGenericUI
else:unix: LIBS += -L$$OUT_PWD/../extern/GenericUI/core/ -lGenericUI

INCLUDEPATH += $$PWD/../extern/GenericUI/core
DEPENDPATH += $$PWD/../extern/GenericUI/core

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../extern/GenericUI/curses/release/ -lGenericNCursesUI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../extern/GenericUI/curses/debug/ -lGenericNCursesUI
else:unix: LIBS += -L$$OUT_PWD/../extern/GenericUI/curses/ -lGenericNCursesUI

INCLUDEPATH += $$PWD/../extern/GenericUI/curses
DEPENDPATH += $$PWD/../extern/GenericUI/curses
