#! /bin/sh

PKG_NAME=Clutter-Gtk
TEST_TYPE=-d
FILE=clutter-gtk

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

test $TEST_TYPE $FILE || {
        echo "You must run this script in the top-level $PROJECT directory"
        exit 1
}

gtkdocize || exit $?

# we need to patch gtk-doc.make to support pretty output with
# libtool 1.x.  Should be fixed in the next version of gtk-doc.
# To be more resilient with the various versions of gtk-doc one
# can find, just sed gkt-doc.make rather than patch it.
sed -e 's#) --mode=compile#) --tag=CC --mode=compile#' gtk-doc.make > gtk-doc.temp \
        && mv gtk-doc.temp gtk-doc.make
sed -e 's#) --mode=link#) --tag=CC --mode=link#' gtk-doc.make > gtk-doc.temp \
        && mv gtk-doc.temp gtk-doc.make

ACLOCAL="${ACLOCAL-aclocal} $ACLOCAL_FLAGS" autoreconf -v --install || exit $?

./configure "$@" && echo Now type make to compile $PKG_NAME.
