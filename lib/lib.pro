#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T10:06:10
#
#-------------------------------------------------

QT       += xml

QT       -= gui

TARGET = NexusCoordinator
TEMPLATE = lib

DEFINES += NEXUSCOORDINATOR_LIBRARY

SOURCES += nexuscoordinator.cpp

HEADERS += nexuscoordinator.h\
        nexuscoordinator_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusComm/release/ -lNexusComm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusComm/debug/ -lNexusComm
else:unix: LIBS += -L$$OUT_PWD/../extern/NexusComm/ -lNexusComm

INCLUDEPATH += $$PWD/../extern/NexusComm
DEPENDPATH += $$PWD/../extern/NexusComm
