#-------------------------------------------------
#
# Project created by QtCreator 2013-07-28T15:28:08
#
#-------------------------------------------------

QT       -= gui

TARGET = ModuleManagment
TEMPLATE = lib

DEFINES += MODULEMANAGMENT_LIBRARY

# Versioning
exists($$PWD/../../extern/GitProjectVersionQt/version.pri) : include($$PWD/../../extern/GitProjectVersionQt/version.pri)

SOURCES += modulemanagment.cpp

HEADERS += modulemanagment.h\
        modulemanagment_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

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
