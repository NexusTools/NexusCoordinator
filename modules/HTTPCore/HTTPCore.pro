#-------------------------------------------------
#
# Project created by QtCreator 2013-07-27T08:24:15
#
#-------------------------------------------------

# Qt Modules
QT       -= gui

# Project Information
TARGET = HTTPCore
TEMPLATE = lib

DEFINES += HTTPCORE_LIBRARY

# Versioning
exists($$PWD/../../version.pri) : include($$PWD/../../version.pri)

# Project Files
SOURCES += httpprocessor.cpp

HEADERS += httpprocessor.h\
        httpcore_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
