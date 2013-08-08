#-------------------------------------------------
#
# Project created by QtCreator 2013-08-05T00:15:24
#
#-------------------------------------------------

QT       -= core
QT       -= gui

TARGET	= nc-shell
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

unix:!symbian {
	maemo5 {
		target.path = /opt/usr/bin
	} else {
		target.path = /bin
	}
	INSTALLS += target
}
