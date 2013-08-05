TEMPLATE = subdirs

!CONFIG(NOEXTERN): SUBDIRS = extern

SUBDIRS +=  lib \
	modules \
	services \
	daemon


!CONFIG(NOCONSOLE)|!CONFIG(NOSHELL): SUBDIRS += console shell
!CONFIG(NOGUI):packagesExist(QtGui): SUBDIRS += gui
