#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T10:06:10
#
#-------------------------------------------------

# Qt Modules
QT       -= gui

!greaterThan(QT_MAJOR_VERSION, 4) {
	DEFINES += LEGACY_QT
}

# Project Information
TARGET = NexusCoordinator
TEMPLATE = lib

CONFIG(debug, debug|release): DEFINES += IDE_MODE DEBUG_MODE

# Versioning
exists($$PWD/../extern/GitProjectVersionQt/version.pri) : include($$PWD/../extern/GitProjectVersionQt/version.pri)

# Project Files
SOURCES += nexuscoordinator.cpp

HEADERS += nexuscoordinator.h\
    coordinatorservice.h \
    coordinator-macros.h

# Internal libraries
exists($$PWD/../extern/NexusConfig/NexusConfig.pro) {
	# Configuration Library
	win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusConfig/release/ -lNexusConfig0
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusConfig/debug/ -lNexusConfig0
	else:unix: LIBS += -L$$OUT_PWD/../extern/NexusConfig/ -lNexusConfig

	INCLUDEPATH += $$PWD/../extern/NexusConfig
	DEPENDPATH += $$PWD/../extern/NexusConfig
}

exists($$PWD/../extern/ModularCore/ModularCore.pro) {
	# Configuration Library
	win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../extern/ModularCore/release/ -lModularCore0
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../extern/ModularCore/debug/ -lModularCore0
	else:unix: LIBS += -L$$OUT_PWD/../extern/ModularCore/ -lModularCore

	INCLUDEPATH += $$PWD/../extern/ModularCore
	DEPENDPATH += $$PWD/../extern/ModularCore
}
