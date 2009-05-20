-*- mode: autoconf -*-
AC_PREREQ(2.52)
AC_INIT(libglade-panda, 1.9.0)
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST
AM_MAINTAINER_MODE

m4_define([required_libxml_version],   [2.4.10])
m4_define([required_glib_version],     [2.5.0])
m4_define([required_gmodule_version],  [2.0.0])
m4_define([required_atk_version],      [1.9.0])
m4_define([required_gtk_version],      [2.5.0])
m4_define([required_gtkpanda_version], [1.9.0])

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

AM_PATH_GTK_2_0(required_gtk_version)

save_LIBS=$LIBS
LIBS="$GTK_LIBS $LIBS"
AC_CHECK_FUNC(gtk_plug_get_type,
  AC_DEFINE(HAVE_GTK_PLUG,1,[gtk has GtkPlug/GtkSocket implementation]))
LIBS=$save_LIBS

PKG_CHECK_MODULES(LIBGLADE_PANDA, [dnl
  libxml-2.0 >= required_libxml_version dnl
  atk >= required_atk_version dnl
  gtk+-2.0 >= required_gtk_version dnl
  glib-2.0 >= required_glib_version dnl
  gmodule-2.0 >= required_gmodule_version dnl
  gtkpanda2 >= required_gtkpanda_version dnl]
  )
AC_DEFINE([USE_PANDA], 1 , "")

AC_CHECK_LIB(gtkpanda-2.0, gtk_panda_pdf_new,
    enable_pdf=1,
    enable_pdf=0)
if test $enable_pdf = 1 ; then
    AC_DEFINE(USE_PDF, 1, "")
fi

AM_GNU_GETTEXT
AM_GNU_GETTEXT_VERSION(0.16.1)

AC_CONFIG_FILES([ 
Makefile
glade/Makefile
intl/Makefile
po/Makefile.in
test/Makefile
test/basic/Makefile
test/gtkpanda/Makefile
libglade-panda-2.0.pc
])

AC_OUTPUT
