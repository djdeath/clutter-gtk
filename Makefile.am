SUBDIRS = clutter-gtk doc examples

clutter-gtk-0.4.pc: clutter-gtk.pc
	@cp -f clutter-gtk.pc clutter-gtk-0.4.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = clutter-gtk-0.4.pc

CLEANFILES = clutter-gtk-0.4.pc

DISTCLEANFILES = clutter-gtk.pc

EXTRA_DIST = clutter-gtk.pc.in
