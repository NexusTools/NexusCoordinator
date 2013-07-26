#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T10:08:53
#
#-------------------------------------------------

# Qt Modules
QT       -= gui

# Versioning
VER_MIN = 1
exists($$PWD/../version.pri) : include($$PWD/../version.pri)

# Project Information
TARGET = NexusCoordinatorConsole
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp
