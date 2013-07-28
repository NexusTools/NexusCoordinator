#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T10:08:53
#
#-------------------------------------------------

# Qt Modules
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
SOURCES += main.cpp
