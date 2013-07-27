#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T10:06:55
#
#-------------------------------------------------

# Qt Modules
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Versioning
exists($$PWD/../version.pri) : include($$PWD/../version.pri)

# Project Information
TARGET = NexusCoordinatorGUI
TEMPLATE = app

# Project Files
SOURCES += main.cpp\
        nexuscoordinatorgui.cpp

HEADERS  += nexuscoordinatorgui.h

FORMS    += nexuscoordinatorgui.ui
