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
exists($$PWD/../extern/GitProjectVersionQt/version.pri) : include($$PWD/../extern/GitProjectVersionQt/version.pri)

# Project Files
SOURCES += main.cpp

OTHER_FILES += \
	default.xml

RESOURCES += \
	resources.qrc

# Internal Includes
exists($$PWD/../lib/lib.pro) {
	# NexusCoordinator Library
	win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lNexusCoordinator0
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lNexusCoordinator0
	else:unix: LIBS += -L$$OUT_PWD/../lib/ -lNexusCoordinator

	INCLUDEPATH += $$PWD/../lib
	DEPENDPATH += $$PWD/../lib
}

exists($$PWD/../lib/lib.pro) {
	# Communication Library
	win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusComm/release/
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusComm/debug/
	else:unix: LIBS += -L$$OUT_PWD/../extern/NexusComm
}

exists($$PWD/../extern/NexusConfig/NexusConfig.pro) {
	# Configuration Library
	win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusConfig/release/ -lNexusConfig0
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../extern/NexusConfig/debug/ -lNexusConfig0
	else:unix: LIBS += -L$$OUT_PWD/../extern/NexusConfig/ -lNexusConfig

	INCLUDEPATH += $$PWD/../extern/NexusConfig
	DEPENDPATH += $$PWD/../extern/NexusConfig
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../extern/ModularCore/release/ -lModularCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../extern/ModularCore/debug/ -lModularCore
else:unix: LIBS += -L$$OUT_PWD/../extern/ModularCore/ -lModularCore

INCLUDEPATH += $$PWD/../extern/ModularCore
DEPENDPATH += $$PWD/../extern/ModularCore
