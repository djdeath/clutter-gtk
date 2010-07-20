include $(top_srcdir)/build/autotools/Makefile.am.silent

NULL =

CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =

INCLUDES = -I$(srcdir) -I$(top_srcdir) -I $(top_builddir)

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"Clutter-Gtk\" 	\
	-DPREFIX=\""$(prefix)"\" 	\
	-DLIBDIR=\""$(libdir)"\"	\
	-DCLUTTER_GTK_COMPILATION	\
	-DG_DISABLE_DEPRECATED		\
	-DGDK_PIXBUF_DISABLE_DEPRECATED	\
	-DGDK_DISABLE_DEPRECATED	\
	-DGTK_DISABLE_DEPRECATED	\
	-DCLUTTER_DISABLE_DEPRECATED	\
	$(CLUTTER_GTK_DEBUG_CFLAGS) 	\
	$(NULL)

AM_CFLAGS = $(MAINTAINER_CFLAGS) $(CLUTTER_CFLAGS) $(GTK_CFLAGS)

lib_LTLIBRARIES = libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la

source_c = \
	$(srcdir)/gtk-clutter-actor.c 		\
	$(srcdir)/gtk-clutter-embed.c 		\
	$(srcdir)/gtk-clutter-offscreen.c	\
	$(srcdir)/gtk-clutter-util.c 		\
	$(srcdir)/gtk-clutter-window.c		\
	$(NULL)

source_h_public = \
	$(top_srcdir)/clutter-gtk/gtk-clutter-actor.h 	\
	$(top_srcdir)/clutter-gtk/gtk-clutter-embed.h 	\
	$(top_srcdir)/clutter-gtk/gtk-clutter-util.h 	\
	$(top_srcdir)/clutter-gtk/gtk-clutter-window.h	\
	$(NULL)

source_h_private = \
	$(srcdir)/gtk-clutter-offscreen.h	\
	$(NULL)

# please, keep the list sorted alphabetically
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_SOURCES = $(source_c) $(source_h) $(source_h_priv)
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_LIBADD  = $(CLUTTER_LIBS) $(GTK_LIBS)
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_LDFLAGS = $(CLUTTER_LT_LDFLAGS)

cluttergtkheadersdir = $(includedir)/clutter-gtk-@CLUTTER_GTK_API_VERSION@/clutter-gtk

# please, keep the list sorted alphabetically
cluttergtkheaders_HEADERS = \
	$(top_srcdir)/clutter-gtk/clutter-gtk.h 		\
	$(top_builddir)/clutter-gtk/gtk-clutter-version.h	\
	$(source_h_public)					\
	$(NULL)

EXTRA_DIST += gtk-clutter-version.h.in

INTROSPECTION_GIRS =

-include $(INTROSPECTION_MAKEFILE)

if HAVE_INTROSPECTION

GtkClutter-1.0.gir: $(INTROSPECTION_SCANNER) Makefile

GtkClutter_1_0_gir_SCANNERFLAGS = --strip-prefix=GtkClutter --add-include-path=$(top_builddir)
GtkClutter_1_0_gir_INCLUDES     = Gtk-3.0 Clutter-1.0
GtkClutter_1_0_gir_PACKAGES     = gtk+-3.0 clutter-1.0
GtkClutter_1_0_gir_CFLAGS       = $(INCLUDES) $(AM_CPPFLAGS)
GtkClutter_1_0_gir_LIBS         = libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la
GtkClutter_1_0_gir_FILES        = $(source_h_public) $(source_c)

INTROSPECTION_GIRS += GtkClutter-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelibs_DATA)
endif
