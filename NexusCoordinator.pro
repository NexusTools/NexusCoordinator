TEMPLATE = subdirs

!CONFIG(NOEXTERN): SUBDIRS = extern

SUBDIRS +=  lib \
	modules \
	services \
	daemon

!CONFIG(NOCONSOLE):packagesExist(ncurses): SUBDIRS += console
!CONFIG(NOGUI):packagesExist(QtGui): SUBDIRS += gui
