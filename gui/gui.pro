#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T10:06:55
#
#-------------------------------------------------

# Qt Modules
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Versioning
exists($$PWD/../extern/GitProjectVersionQt/version.pri) : include($$PWD/../extern/GitProjectVersionQt/version.pri)

# Project Information
TARGET = nc-qt
TEMPLATE = app

# Project Files
SOURCES += main.cpp\
        nexuscoordinatorgui.cpp

HEADERS  += nexuscoordinatorgui.h

FORMS    += nexuscoordinatorgui.ui

unix:!symbian {
	maemo5 {
		target.path = /opt/usr/bin
	} else {
		target.path = /usr/bin
	}
	INSTALLS += target
}
