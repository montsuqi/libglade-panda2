## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-I$(top_srcdir)/ $(LIBGLADE_PANDA_CFLAGS)

noinst_PROGRAMS = gtkpandatest

gtkpandatest_SOURCES = \
	main.c \
	callbacks.c callbacks.h

gtkpandatest_LDADD = \
	$(LIBGLADE_PANDA_LIBS)	\
	$(top_builddir)/glade/libglade-panda-2.0.la

EXTRA_DIST = gtkpanda.glade test.pdf
