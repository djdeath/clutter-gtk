#! /bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

ORIGDIR=`pwd`

cd $srcdir
PROJECT=Clutter-GTK
TEST_TYPE=-f
FILE=clutter-gtk/clutter-gtk.h

test $TEST_TYPE $FILE || {
        echo "You must run this script in the top-level $PROJECT directory"
        exit 1
}

AUTOMAKE=
ACLOCAL=
if automake-1.11 --version < /dev/null > /dev/null 2>&1 ; then
        AUTOMAKE=automake-1.11
        ACLOCAL=aclocal-1.11
else
        echo
        echo "You must have automake 1.11.x installed to compile $PROJECT
ECT."
        echo "Install the appropriate package for your distribution,"
        echo "or get the source tarball at http://ftp.gnu.org/gnu/automake/"
        exit 1
fi

(gtkdocize --version) < /dev/null > /dev/null 2>&1 || {
        echo
        echo "You must have gtk-doc installed to compile $PROJECT."
        echo "Install the appropriate package for your distribution,"
        echo "or get the source tarball at http://ftp.gnome.org/pub/GNOME/sources/gtk-doc/"
        exit 1
}

# NOCONFIGURE is used by gnome-common
if test -z "$NOCONFIGURE"; then
        if test -z "$*"; then
                echo "I am going to run ./configure with no arguments - if you wish "
                echo "to pass any to it, please specify them on the $0 command line."
        fi
fi

rm -rf autom4te.cache

gtkdocize || exit $?
ACLOCAL="${ACLOCAL-aclocal} $ACLOCAL_FLAGS" AUTOMAKE=${AUTOMAKE} autoreconf -v --install || exit $?
cd $ORIGDIR || exit $?

if test -z "$NOCONFIGURE"; then
        $srcdir/configure $AUTOGEN_CONFIGURE_ARGS "$@" || exit $?
        echo "Now type 'make' to compile $PROJECT."
fi
