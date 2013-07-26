#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T10:06:10
#
#-------------------------------------------------

# Qt Modules
QT       -= gui

# Versioning
VER_MIN = 1
exists($$PWD/../version.pri) : include($$PWD/../version.pri)

# Project Information
TARGET = NexusCoordinator
TEMPLATE = lib

DEFINES += NEXUSCOORDINATOR_LIBRARY

SOURCES += nexuscoordinator.cpp \
    coordinatorservice.cpp

HEADERS += nexuscoordinator.h\
        nexuscoordinator_global.h \
    coordinatorservice.h
