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
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_LIBADD = $(CLUTTER_LIBS) $(GTK_LIBS)
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_LDFLAGS = $(CLUTTER_LT_LDFLAGS)

cluttergtkheadersdir = $(includedir)/clutter-gtk-@CLUTTER_GTK_API_VERSION@/clutter-gtk

# please, keep the list sorted alphabetically
cluttergtkheaders_HEADERS = \
	$(top_srcdir)/clutter-gtk/clutter-gtk.h 		\
	$(top_builddir)/clutter-gtk/gtk-clutter-version.h	\
	$(source_h_public)					\
	$(NULL)

EXTRA_DIST += gtk-clutter-version.h.in

if HAVE_INTROSPECTION
BUILT_GIRSOURCES =

GtkClutter-@CLUTTER_GTK_API_VERSION@.gir: $(INTROSPECTION_SCANNER) libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la
	$(QUIET_GEN)$(INTROSPECTION_SCANNER) -v \
		--namespace GtkClutter --nsversion=@CLUTTER_GTK_API_VERSION@ \
		$(INCLUDES) \
		$(AM_CPPFLAGS) \
		--add-include-path=$(srcdir) --add-include=path=. \
		--include=Clutter-1.0 \
		--include=Gtk-2.0 \
		--library=libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la \
		--libtool="$(top_builddir)/libtool" \
		--output $@ \
		--pkg gobject-2.0 \
		--pkg clutter-1.0 \
		--pkg gtk+-2.0 \
		$(source_h_public) $(source_c)

BUILT_GIRSOURCES += GtkClutter-@CLUTTER_GTK_API_VERSION@.gir

# INTROSPECTION_GIRDIR/INTROSPECTION_TYPELIBDIR aren't the right place to
# install anything - we need to install inside our prefix.
girdir = $(datadir)/gir-1.0
gir_DATA = $(BUILT_GIRSOURCES)

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = $(BUILT_GIRSOURCES:.gir=.typelib)

%.typelib: %.gir $(INTROSPECTION_COMPILER)
	$(QUIET_GEN)\
	LD_LIBRARY_PATH=.libs$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH} \
	$(INTROSPECTION_COMPILER) \
		--includedir=$(srcdir) \
		--includedir=. \
		$(INTROSPECTION_COMPILER_OPTS) \
	$< -o $(builddir)/$(@F)

CLEANFILES += $(BUILT_GIRSOURCES) $(typelibs_DATA)
endif
