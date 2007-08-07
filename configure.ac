# clutter package version number, (as distinct from shared library version)
# An odd micro number indicates in-progress development, (eg. from CVS)
# An even micro number indicates a released version.
m4_define([clutter_major_version], [0])
m4_define([clutter_minor_version], [4])
m4_define([clutter_micro_version], [0])

m4_define([clutter_version],
          [clutter_major_version.clutter_minor_version.clutter_micro_version])

m4_define([clutter_api_version],
          [clutter_major_version.clutter_minor_version])

# increase the interface age for each release; if the API changes, set to 0
m4_define([clutter_interface_age], [0])
m4_define([clutter_binary_age],
          [m4_eval(100 * clutter_minor_version + clutter_micro_version)])

AC_PREREQ(2.53)

AC_INIT([clutter-gtk],
        [clutter_version],
        [http://bugzilla.o-hand.com/enter_bug.cgi?product=Clutter])
AC_CONFIG_SRCDIR([clutter-gtk/clutter-gtk.h])
AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([1.7])

CLUTTER_GTK_MAJOR_VERSION=clutter_major_version
CLUTTER_GTK_MINOR_VERSION=clutter_minor_version
CLUTTER_GTK_MICRO_VERSION=clutter_micro_version
CLUTTER_GTK_VERSION=clutter_version
AC_SUBST(CLUTTER_GTK_MAJOR_VERSION)
AC_SUBST(CLUTTER_GTK_MINOR_VERSION)
AC_SUBST(CLUTTER_GTK_MICRO_VERSION)
AC_SUBST(CLUTTER_GTK_VERSION)

m4_define([lt_current],
          [m4_eval(100 * clutter_minor_version + clutter_micro_version - clutter_interface_age)])
m4_define([lt_revision], [clutter_interface_age])
m4_define([lt_age], [m4_eval(clutter_binary_age - clutter_interface_age)])
CLUTTER_LT_CURRENT=lt_current
CLUTTER_LT_REV=lt_revision
CLUTTER_LT_AGE=lt_age
CLUTTER_LT_VERSION="$CLUTTER_LT_CURRENT:$CLUTTER_LT_REV:$CLUTTER_LT_AGE"
CLUTTER_LT_LDFLAGS="-version-info $CLUTTER_LT_VERSION"

AC_SUBST(CLUTTER_LT_VERSION)
AC_SUBST(CLUTTER_LT_LDFLAGS)

dnl ========================================================================

# Checks for programs.
AC_PROG_CC
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([memset munmap strcasecmp strdup])

CLUTTER_REQUIRED=0.4.0

PKG_CHECK_MODULES(CLUTTER, clutter-glx-0.4 >= $CLUTTER_REQUIRED)
AC_SUBST(CLUTTER_CFLAGS)
AC_SUBST(CLUTTER_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-2.0)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

dnl ========================================================================

if test "x$GCC" = "xyes"; then
        GCC_FLAGS="-g -Wall"
fi

dnl = Enable debug level ===================================================

m4_define([debug_default],
          m4_if(m4_eval(clutter_minor_version % 2), [1], [yes], [minimum]))

AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                             [turn on debugging. yes; All glib asserts, checks and runtime clutter verbose messages. minimum; Just glib cast checks and runtime clutter verbose messagaes. no; No glib asserts or checks and no runtime clutter verbose messages.  @<:@default=debug_default@:>@]),
,
              enable_debug=debug_default)

if test "x$enable_debug" = "xyes"; then
  test "$cflags_set" = set || CFLAGS="$CFLAGS -g"
  CLUTTER_DEBUG_CFLAGS="-DCLUTTER_ENABLE_DEBUG"
else
  if test "x$enable_debug" = "xno"; then
    CLUTTER_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
  else # minimum
    CLUTTER_DEBUG_CFLAGS="-DCLUTTER_ENABLE_DEBUG -DG_DISABLE_CAST_CHECKS"
  fi
fi

AC_SUBST(CLUTTER_DEBUG_CFLAGS)

dnl = GTK Doc check ========================================================

GTK_DOC_CHECK([1.4])

dnl ========================================================================

AC_SUBST(GCC_FLAGS)

AC_CONFIG_FILES([
        Makefile
        clutter-gtk/Makefile
        doc/Makefile
        doc/reference/Makefile
        doc/reference/version.xml
        examples/Makefile
        clutter-gtk.pc
])

AC_OUTPUT

dnl ========================================================================

echo ""
echo "                 ClutterGTK $VERSION"
echo "                 ==================="
echo ""
echo "                   prefix:   ${prefix}"
echo ""
echo "              Debug level:   ${enable_debug}"
echo "            Documentation:   ${enable_gtk_doc}"
echo ""
