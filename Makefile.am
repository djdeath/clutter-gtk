SUBDIRS = clutter-gtk doc examples

clutter-gtk-0.7.pc: clutter-gtk.pc
	@cp -f clutter-gtk.pc clutter-gtk-0.7.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = clutter-gtk-0.7.pc

CLEANFILES = clutter-gtk-0.7.pc

DISTCLEANFILES = clutter-gtk.pc

EXTRA_DIST = clutter-gtk.pc.in

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc
