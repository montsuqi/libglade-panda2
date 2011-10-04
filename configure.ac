-*- mode: autoconf -*-
AC_PREREQ(2.52)
AC_INIT(libglade-panda, 3.0.0)
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
AM_MAINTAINER_MODE


AC_CONFIG_SRCDIR([glade/glade.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

# make sure $ACLOCAL_FLAGS are used during a rebuild.
AC_SUBST([ACLOCAL_AMFLAGS], ["-I $ac_macro_dir \${ACLOCAL_FLAGS}"])

AC_ARG_ENABLE(debug,
  AC_HELP_STRING([--enable-debug], [check LIBGLADE_PANDA_DEBUG env var]),,
  [enable_debug=no])

AC_MSG_CHECKING([if debuging support was requested])
if test "x$enable_debug" != xno; then
  AC_DEFINE(DEBUG, 1, [enable use of LIBGLADE_PANDA_DEBUG environment variable])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi


AC_PROG_CC
if test "$GCC" = "yes"; then
        CFLAGS="-g -Wall -Wunused -funsigned-char -Wno-pointer-sign"
fi
AC_GNU_SOURCE
AC_ISC_POSIX
AC_HEADER_STDC
AC_PROG_LIBTOOL

save_LIBS=$LIBS
LIBS="$GTK_LIBS $LIBS"
AC_CHECK_FUNC(gtk_plug_get_type,
  AC_DEFINE(HAVE_GTK_PLUG,1,[gtk has GtkPlug/GtkSocket implementation]))
LIBS=$save_LIBS

PKG_CHECK_MODULES(LIBGLADE_PANDA, [dnl
  libxml-2.0 >= 2.4.10 dnl
  gtkpanda2 >= 3.0.0 dnl]
  )
AC_DEFINE([USE_PANDA], 1 , "")

AC_CONFIG_FILES([ 
Makefile
glade/Makefile
test/Makefile
libglade-panda-2.0.pc
])

AC_OUTPUT
