#-------------------------------------------------
#
# Project created by QtCreator 2013-07-28T17:24:27
#
#-------------------------------------------------

QT       -= gui

TARGET = FTPServer
TEMPLATE = lib

DEFINES += FTPSERVER_LIBRARY

# Versioning
exists($$PWD/../../extern/GitProjectVersionQt/version.pri) : include($$PWD/../../extern/GitProjectVersionQt/version.pri)

SOURCES += ftpserver.cpp

HEADERS += ftpserver.h\
		ftpserver_global.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../extern/ModularCore/release/ -lModularCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../extern/ModularCore/debug/ -lModularCore
else:unix: LIBS += -L$$OUT_PWD/../../extern/ModularCore/ -lModularCore

INCLUDEPATH += $$PWD/../../extern/ModularCore
DEPENDPATH += $$PWD/../../extern/ModularCore

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -lNexusCoordinator
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -lNexusCoordinator
else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lNexusCoordinator

INCLUDEPATH += $$PWD/../../lib
DEPENDPATH += $$PWD/../../lib
