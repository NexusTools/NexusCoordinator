#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T10:07:50
#
#-------------------------------------------------

# Qt Modules
QT       -= gui

# Project Information
TARGET = NexusCoordinatorDaemon
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# Versioning
VER_MIN = 1
exists($$PWD/../version.pri) : include($$PWD/../version.pri)

# Files
SOURCES += main.cpp

# Internal Includes
exists($$PWD/../lib/lib.pro) {
	# NexusCoordinator Library
	win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lNexusCoordinator
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lNexusCoordinator
	else:unix: LIBS += -L$$OUT_PWD/../lib/ -lNexusCoordinator

	INCLUDEPATH += $$PWD/../lib
	DEPENDPATH += $$PWD/../lib
}

exists($$PWD/../extern/NexusConfig/NexusConfig.pro) {
	# Configuration Library
	win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusConfig/release/ -lNexusConfig
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusConfig/debug/ -lNexusConfig
	else:unix: LIBS += -L$$OUT_PWD/../extern/NexusConfig/ -lNexusConfig

	INCLUDEPATH += $$PWD/../extern/NexusConfig
	DEPENDPATH += $$PWD/../extern/NexusConfig
}

OTHER_FILES += \
    default.xml

RESOURCES += \
    resources.qrc
