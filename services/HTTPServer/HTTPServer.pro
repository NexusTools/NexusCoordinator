#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T13:39:36
#
#-------------------------------------------------

QT       -= gui

TARGET = HTTPServer
TEMPLATE = lib

DEFINES += EXPORT_DECL

# Versioning
exists($$PWD/../../extern/GitProjectVersionQt/version.pri) : include($$PWD/../../extern/GitProjectVersionQt/version.pri)

# Project Files
SOURCES += httpserver.cpp

HEADERS += httpserver.h \
    global.h

OTHER_FILES += \
	library.xml

RESOURCES += \
	resources.qrc

# Internal Libraries
exists($$PWD/../../lib/lib.pro) {
	# Main Library
        win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -lNexusCoordinator0
        else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -lNexusCoordinator0
	else:unix: LIBS += -L$$OUT_PWD/../../lib/ -lNexusCoordinator

	INCLUDEPATH += $$PWD/../../lib
	DEPENDPATH += $$PWD/../../lib
}

exists($$PWD/../../extern/NexusComm/NexusComm.pro) {
	# Communication Library
        win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../extern/NexusComm/release/ -lNexusComm0
        else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../extern/NexusComm/debug/ -lNexusComm0
	else:unix: LIBS += -L$$OUT_PWD/../../extern/NexusComm/ -lNexusComm

	INCLUDEPATH += $$PWD/../../extern/NexusComm
	DEPENDPATH += $$PWD/../../extern/NexusComm
}

exists($$PWD/../../modules/HTTPCore/HTTPCore.pro) {
	# HTTP Transports and Processors
	win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../modules/HTTPCore/release/ -lHTTPCore0
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../modules/HTTPCore/debug/ -lHTTPCore0
	else:unix: LIBS += -L$$OUT_PWD/../../modules/HTTPCore/ -lHTTPCore

	INCLUDEPATH += $$PWD/../../modules/HTTPCore
	DEPENDPATH += $$PWD/../../modules/HTTPCore
}

exists($$PWD/../../extern/ModularCore/ModularCore.pro) {
	# Modules and Plugins
	win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../extern/ModularCore/release/ -lModularCore
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../extern/ModularCore/debug/ -lModularCore
	else:unix: LIBS += -L$$OUT_PWD/../../extern/ModularCore/ -lModularCore

	INCLUDEPATH += $$PWD/../../extern/ModularCore
	DEPENDPATH += $$PWD/../../extern/ModularCore
}
