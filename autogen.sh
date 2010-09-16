#! /bin/sh

PKG_NAME=Clutter-Gtk
TEST_TYPE=-f
FILE=clutter-gtk/clutter-gtk.h

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

test $TEST_TYPE $FILE || {
        echo "You must run this script in the top-level $PROJECT directory"
        exit 1
}

gtkdocize || exit $?

ACLOCAL="${ACLOCAL-aclocal} $ACLOCAL_FLAGS" autoreconf -v --install || exit $?

./configure "$@" && echo Now type make to compile $PKG_NAME.
