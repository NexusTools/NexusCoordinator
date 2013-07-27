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

DEFINES += NEXUSCOORDINATOR_LIBRARY
CONFIG(debug, debug|release): DEFINES += IDE_MODE DEBUG_MODE

# Versioning
VER_MIN = 1
exists($$PWD/../version.pri) : include($$PWD/../version.pri)

# Project Files
SOURCES += nexuscoordinator.cpp

HEADERS += nexuscoordinator.h\
    coordinatorservice.h \
    coordinatorlibrary.h \
    libraryhelper.h

# Internal libraries
exists($$PWD/../extern/NexusConfig/NexusConfig.pro) {
	# Configuration Library
	win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusConfig/release/ -lNexusConfig
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusConfig/debug/ -lNexusConfig
	else:unix: LIBS += -L$$OUT_PWD/../extern/NexusConfig/ -lNexusConfig

	INCLUDEPATH += $$PWD/../extern/NexusConfig
	DEPENDPATH += $$PWD/../extern/NexusConfig
}
