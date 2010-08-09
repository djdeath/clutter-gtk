# clutter package version number, (as distinct from shared library version)
# An odd micro number indicates in-progress development, (eg. from git)
# An even micro number indicates a released version.
m4_define([clutter_gtk_major], [0])
m4_define([clutter_gtk_minor], [90])
m4_define([clutter_gtk_micro], [3])

m4_define([clutter_gtk_api_version], [1.0])
m4_define([clutter_gtk_version], [clutter_gtk_major.clutter_gtk_minor.clutter_gtk_micro])

# increase the interface age for each release; if the API changes, set to 0
m4_define([clutter_gtk_interface_age], [0])
m4_define([clutter_gtk_binary_age], [m4_eval(100 * clutter_gtk_minor + clutter_gtk_micro)])

AC_PREREQ([2.63])

AC_INIT([clutter-gtk],
        [clutter_gtk_version],
        [http://bugzilla.clutter-project.org/enter_bug.cgi?product=clutter-gtk],
        [clutter-gtk],
        [http://www.clutter-project.org])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([clutter-gtk/clutter-gtk.h])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build/autotools])

AM_INIT_AUTOMAKE([1.11 foreign -Wno-portability no-define dist-bzip2])

AM_SILENT_RULES([yes])

CLUTTER_GTK_MAJOR_VERSION=clutter_gtk_major
CLUTTER_GTK_MINOR_VERSION=clutter_gtk_minor
CLUTTER_GTK_MICRO_VERSION=clutter_gtk_micro
CLUTTER_GTK_API_VERSION=clutter_gtk_api_version
CLUTTER_GTK_VERSION=clutter_gtk_version
AC_SUBST(CLUTTER_GTK_MAJOR_VERSION)
AC_SUBST(CLUTTER_GTK_MINOR_VERSION)
AC_SUBST(CLUTTER_GTK_MICRO_VERSION)
AC_SUBST(CLUTTER_GTK_API_VERSION)
AC_SUBST(CLUTTER_GTK_VERSION)

m4_define([lt_current], [m4_eval(100 * clutter_gtk_minor + clutter_gtk_micro - clutter_gtk_interface_age)])
m4_define([lt_revision], [clutter_gtk_interface_age])
m4_define([lt_age], [m4_eval(clutter_gtk_binary_age - clutter_gtk_interface_age)])
CLUTTER_GTK_LT_CURRENT=lt_current
CLUTTER_GTK_LT_REV=lt_revision
CLUTTER_GTK_LT_AGE=lt_age
CLUTTER_GTK_LT_VERSION="$CLUTTER_GTK_LT_CURRENT:$CLUTTER_GTK_LT_REV:$CLUTTER_GTK_LT_AGE"
CLUTTER_GTK_LT_LDFLAGS="-version-info $CLUTTER_GTK_LT_VERSION"

AC_SUBST(CLUTTER_GTK_LT_VERSION)
AC_SUBST(CLUTTER_GTK_LT_LDFLAGS)

dnl ========================================================================

# Checks for programs.
AM_PROG_CC_C_O

# require libtool >= 2.2
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

# Checks for header files.
AC_HEADER_STDC

m4_define([clutter_req_version], [1.3.8])
m4_define([gtk_req_version], [2.90.5])
m4_define([flavour_default], [x11])
AC_ARG_WITH([flavour],
            [AC_HELP_STRING([--with-flavour=@<:@x11/win32@:>@],
                            [Select the Clutter backend, default=flavour_default])],
            [],
            [with_flavour=flavour_default])

AS_CASE([$with_flavour],
        [x11],
        [
          AC_DEFINE([HAVE_CLUTTER_GTK_X11], 1, [Using the X11 flavour])
          PKG_CHECK_MODULES(CLUTTER, clutter-x11-1.0 >= clutter_req_version)
          PKG_CHECK_MODULES(GTK, gtk+-x11-3.0 >= gtk_req_version)
          CLUTTER_GTK_REQUIRES="clutter-x11-1.0 gtk+-x11-3.0"
        ],

        [win32],
        [
          AC_DEFINE([HAVE_CLUTTER_GTK_WIN32], 1, [Using the Win32 flavour])
          PKG_CHECK_MODULES(CLUTTER, clutter-win32-1.0 >= $CLUTTER_REQUIRED)
          PKG_CHECK_MODULES(GTK, gtk+-win32-3.0 >= gtk_req_version)
          CLUTTER_GTK_REQUIRES="clutter-win32-1.0 gtk+-win32-3.0"
        ]

        [AC_MSG_ERROR([Invalid argument for --with-flavour])]
)

CLUTTER_GTK_FLAVOUR=$with_flavour
AC_SUBST(CLUTTER_GTK_FLAVOUR)
AC_SUBST(CLUTTER_GTK_REQUIRES)
AC_SUBST(CLUTTER_CFLAGS)
AC_SUBST(CLUTTER_LIBS)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

dnl = Enable debug level ===================================================

m4_define([debug_default], [m4_if(m4_eval(clutter_gtk_minor % 2), [1], [yes], [minimum])])
AC_ARG_ENABLE([debug],
              [AC_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                              [Turns on debugging. @<:@default=debug_default@:>@])],
              [],
              [enable_debug=debug_default])

AS_CASE([$enable_debug],

        [yes],
        [
          test "$cflags_set" = set || CFLAGS="$CFLAGS -g"
          CLUTTER_GTK_DEBUG_CFLAGS="-DCLUTTER_GTK_ENABLE_DEBUG"
        ],

        [minimum],
        [
          CLUTTER_GTK_DEBUG_CFLAGS="-DCLUTTER_GTK_ENABLE_DEBUG -DG_DISABLE_CAST_CHECKS"
        ],

        [no],
        [
          CLUTTER_GTK_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
        ],

        [AC_MSG_ERROR([Invalid argument for --enable-debug])]
)

AC_SUBST(CLUTTER_GTK_DEBUG_CFLAGS)

# use strict compiler flags only on development releases
m4_define([maintainer_flags_default], [m4_if(m4_eval(clutter_gtk_minor % 2), [1], [yes], [no])])
AC_ARG_ENABLE([maintainer-flags],
              [AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                              [Use strict compiler flags @<:@default=no@:>@])],
              [],
              [enable_maintainer_flags=maintainer_flags_default])

MAINTAINER_CFLAGS=""
AS_IF([test "x$enable_maintainer_flags" = "xyes" && test "x$GCC" = "xyes"],
      [
        AS_COMPILER_FLAGS([MAINTAINER_CFLAGS],
                          ["-Wall -Wshadow -Wcast-align -Wuninitialized
                            -Wno-strict-aliasing -Wempty-body -Wformat
                            -Wformat-security -Winit-self"])
      ]
)

AC_SUBST(MAINTAINER_CFLAGS)

dnl = GTK Doc check ========================================================

G_PREFIX=`$PKG_CONFIG --variable=prefix glib-2.0`
GDK_PREFIX=`$PKG_CONFIG --variable=prefix gdk-2.0`
GTK_PREFIX=`$PKG_CONFIG --variable=prefix gtk+-2.0`
GDK_PIXBUF_PREFIX=`$PKG_CONFIG --variable=prefix gdk-pixbuf-2.0`
CLUTTER_PREFIX=`$PKG_CONFIG --variable=prefix clutter-1.0`
AC_SUBST(G_PREFIX)
AC_SUBST(GDK_PREFIX)
AC_SUBST(GTK_PREFIX)
AC_SUBST(GDK_PIXBUF_PREFIX)
AC_SUBST(CLUTTER_PREFIX)

GTK_DOC_CHECK([1.14], [--flavour no-tmpl])

GOBJECT_INTROSPECTION_CHECK([0.6.14])

AC_CONFIG_FILES([
        Makefile
        build/Makefile
        build/autotools/Makefile
        clutter-gtk/Makefile
        clutter-gtk/gtk-clutter-version.h
        doc/Makefile
        doc/reference/Makefile
        doc/reference/version.xml
        examples/Makefile
        clutter-gtk.pc
])

AC_OUTPUT

dnl ========================================================================

echo ""
echo "                 Clutter-GTK $VERSION"
echo "                 ===================="
echo ""
echo "                   prefix:   ${prefix}"
echo ""
echo "                  Flavour:   ${CLUTTER_GTK_FLAVOUR}"
echo "              Debug level:   ${enable_debug}"
echo "            Documentation:   ${enable_gtk_doc}"
echo "           Compiler flags:   $MAINTAINER_CFLAGS"
echo "       Introspection data:   ${enable_introspection}"
echo ""
