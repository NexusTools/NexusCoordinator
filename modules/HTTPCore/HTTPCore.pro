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
exists($$PWD/../../extern/GitProjectVersionQt/version.pri) : include($$PWD/../../extern/GitProjectVersionQt/version.pri)

# Project Files
SOURCES += httpprocessor.cpp \
    httppacket.cpp \
    httpserver.cpp

HEADERS += httpprocessor.h\
    httppacket.h \
	global.h \
    httpserver.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../extern/NexusComm/release/ -lNexusComm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../extern/NexusComm/debug/ -lNexusComm
else:unix: LIBS += -L$$OUT_PWD/../../extern/NexusComm/ -lNexusComm

INCLUDEPATH += $$PWD/../../extern/NexusComm
DEPENDPATH += $$PWD/../../extern/NexusComm
