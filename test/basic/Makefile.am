## Process this file with automake to produce Makefile.in

INCLUDES = \
	$(LIBGLADE_PANDA_CFLAGS)

noinst_PROGRAMS = basictest

basictest_SOURCES = \
	main.c \
	callbacks.c callbacks.h

basictest_LDADD = \
	$(LIBGLADE_PANDA_LIBS)	\
	$(top_builddir)/glade/libglade-panda-2.0.la
