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
