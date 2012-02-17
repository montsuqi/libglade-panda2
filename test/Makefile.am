INCLUDES = $(LIBGLADE_PANDA_CFLAGS)
noinst_PROGRAMS = \
	test-libglade-panda \
	panda-glade-convert
test_libglade_panda_LDADD = \
	$(top_builddir)/glade/libglade-panda-2.0.la \
	$(LIBGLADE_PANDA_LIBS)
test_libglade_panda_SOURCES = test-libglade-panda.c

panda_glade_convert_LDADD = \
	$(top_builddir)/glade/libglade-panda-2.0.la \
	$(LIBGLADE_PANDA_LIBS)
panda_glade_convert_SOURCES = panda-glade-convert.c

SUBDIRS = 	basic gtkpanda
