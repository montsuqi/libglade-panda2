
EXTRA_DIST = config.rpath m4/ChangeLog 			\
	libglade-panda-2.0.pc.in

SUBDIRS = 	intl po glade test

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libglade-panda-2.0.pc

ACLOCAL_AMFLAGS = -I m4

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)-$(VERSION)~pre`date +"%y%m%d"`
