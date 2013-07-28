#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T13:38:20
#
#-------------------------------------------------

QT       -= gui

TARGET = ControlServer
TEMPLATE = lib

DEFINES += EXPORT_DECL

# Versioning
exists($$PWD/../../extern/GitProjectVersionQt/version.pri) : include($$PWD/../../extern/GitProjectVersionQt/version.pri)

# Project Files
SOURCES += controlserver.cpp

HEADERS += controlserver.h \
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
