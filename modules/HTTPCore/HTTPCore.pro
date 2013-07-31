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
SOURCES += \
    httppacket.cpp \
    httpservercore.cpp \
    module.cpp

HEADERS +=\
    httppacket.h \
	global.h \
    httpservercore.h \
    httprequesthandler.h \
    httpabstractprocessor.h \
	httpprocessor.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../extern/NexusComm/release/ -lNexusComm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../extern/NexusComm/debug/ -lNexusComm
else:unix: LIBS += -L$$OUT_PWD/../../extern/NexusComm/ -lNexusComm

INCLUDEPATH += $$PWD/../../extern/NexusComm
DEPENDPATH += $$PWD/../../extern/NexusComm
