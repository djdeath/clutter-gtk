ACLOCAL_AMFLAGS = -I build/autotools

V         = @
Q         = $(V:1=)
QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

SUBDIRS = build clutter-gtk doc examples

clutter-gtk-0.9.pc: clutter-gtk.pc
	$(QUIET_GEN)cp -f clutter-gtk.pc clutter-gtk-0.9.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = clutter-gtk-0.9.pc

CLEANFILES = clutter-gtk-0.9.pc

DISTCLEANFILES = clutter-gtk.pc

EXTRA_DIST = clutter-gtk.pc.in

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-maintainer-flags
