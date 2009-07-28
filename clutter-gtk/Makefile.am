CLEANFILES=
INCLUDES = \
	-I$(srcdir) 				\
	-I$(top_srcdir) 			\
	-DG_LOG_DOMAIN=\"Clutter-Gtk\" 		\
	-DPREFIX=\""$(prefix)"\" 		\
	-DLIBDIR=\""$(libdir)"\" 		\
        -DCLUTTER_GTK_COMPILATION               \
        -DG_DISABLE_DEPRECATED                  \
        -DGDK_PIXBUF_DISABLE_DEPRECATED         \
        -DGDK_DISABLE_DEPRECATED                \
        -DGTK_DISABLE_DEPRECATED                \
	-DCLUTTER_DISABLE_DEPRECATED

AM_CPPFLAGS = \
	$(CLUTTER_GTK_DEBUG_CFLAGS) \
	$(MAINTAINER_CFLAGS) \
	$(CLUTTER_CFLAGS) \
	$(GTK_CFLAGS)

lib_LTLIBRARIES = libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la

# please, keep the list sorted alphabetically
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_SOURCES = \
	$(srcdir)/gtk-clutter-embed.c 		\
	$(srcdir)/gtk-clutter-scrollable.c 	\
	$(srcdir)/gtk-clutter-util.c 		\
	$(srcdir)/gtk-clutter-viewport.c	\
	$(srcdir)/gtk-clutter-zoomable.c

libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_LIBADD = $(CLUTTER_LIBS) $(GTK_LIBS)
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_LDFLAGS = $(CLUTTER_LT_LDFLAGS)

cluttergtkheadersdir = $(includedir)/clutter-1.0/clutter-gtk

# please, keep the list sorted alphabetically
cluttergtkheaders_HEADERS = \
	$(top_srcdir)/clutter-gtk/clutter-gtk.h 		\
	$(top_srcdir)/clutter-gtk/gtk-clutter-embed.h 		\
	$(top_srcdir)/clutter-gtk/gtk-clutter-scrollable.h 	\
	$(top_srcdir)/clutter-gtk/gtk-clutter-util.h 		\
	$(top_srcdir)/clutter-gtk/gtk-clutter-viewport.h	\
	$(top_srcdir)/clutter-gtk/gtk-clutter-zoomable.h

if HAVE_INTROSPECTION
BUILT_GIRSOURCES =

GtkClutter-@CLUTTER_GTK_API_VERSION@.gir: $(INTROSPECTION_SCANNER) libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la
	$(QUIET_GEN)$(INTROSPECTION_SCANNER) -v \
		--namespace GtkClutter --nsversion=@CLUTTER_GTK_API_VERSION@ \
		$(INCLUDES) \
		--add-include-path=$(srcdir) --add-include=path=. \
		--include=Clutter-1.0 \
		--include=Gtk-2.0 \
		--library=clutter-gtk-@CLUTTER_GTK_API_VERSION@ \
		--libtool="$(top_builddir)/libtool" \
		--output $@ \
		--pkg gobject-2.0 \
		--pkg clutter-1.0 \
		--pkg gtk+-2.0 \
		$(cluttergtkheaders_HEADERS) \
		$(libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_SOURCES)

BUILT_GIRSOURCES += GtkClutter-@CLUTTER_GTK_API_VERSION@.gir

# INTROSPECTION_GIRDIR/INTROSPECTION_TYPELIBDIR aren't the right place to
# install anything - we need to install inside our prefix.
girdir = $(datadir)/gir-1.0
gir_DATA = $(BUILT_GIRSOURCES)

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = $(BUILT_GIRSOURCES:.gir=.typelib)

%.typelib: %.gir $(INTROSPECTION_COMPILER)
	$(QUIET_GEN)$(INTROSPECTION_COMPILER) --includedir=$(srcdir) --includedir=. $(INTROSPECTION_COMPILER_OPTS) $< -o $(builddir)/$(@F)

CLEANFILES += $(BUILT_GIRSOURCES) $(typelibs_DATA)
endif
