CLEANFILES=

INCLUDES = -I$(srcdir) -I$(top_srcdir)

AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"Clutter-Gtk\" 		\
	-DPREFIX=\""$(prefix)"\" 		\
	-DLIBDIR=\""$(libdir)"\" 		\
        -DCLUTTER_GTK_COMPILATION               \
        -DG_DISABLE_DEPRECATED                  \
        -DGDK_PIXBUF_DISABLE_DEPRECATED         \
        -DGDK_DISABLE_DEPRECATED                \
        -DGTK_DISABLE_DEPRECATED                \
	-DCOGL_DISABLE_DEPRECATED		\
	-DCLUTTER_DISABLE_DEPRECATED		\
	$(CLUTTER_GTK_DEBUG_CFLAGS)

common_cflags = $(CLUTTER_CFLAGS) $(GTK_CFLAGS)

# please, keep these lists sorted alphabetically
sources_c = \
	$(srcdir)/gtk-clutter-embed.c 		\
	$(srcdir)/gtk-clutter-scrollable.c 	\
	$(srcdir)/gtk-clutter-util.c 		\
	$(srcdir)/gtk-clutter-viewport.c	\
	$(srcdir)/gtk-clutter-zoomable.c

sources_h = \
	$(srcdir)/gtk-clutter-embed.h 		\
	$(srcdir)/gtk-clutter-scrollable.h 	\
	$(srcdir)/gtk-clutter-util.h 		\
	$(srcdir)/gtk-clutter-viewport.h	\
	$(srcdir)/gtk-clutter-zoomable.h

lib_LTLIBRARIES = libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la

# please, keep the list sorted alphabetically
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_SOURCES = $(sources_c)
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_LIBADD = $(CLUTTER_LIBS) $(GTK_LIBS)
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_LDFLAGS = $(CLUTTER_LT_LDFLAGS)
libclutter_gtk_@CLUTTER_GTK_API_VERSION@_la_CFLAGS = $(common_cflags) $(MAINTAINER_CFLAGS)

cluttergtkheadersdir = $(includedir)/clutter-1.0/clutter-gtk
cluttergtkheaders_HEADERS = $(sources_h) $(srcdir)/clutter-gtk.h

if HAVE_INTROSPECTION
BUILT_GIRSOURCES =

GtkClutter-@CLUTTER_GTK_API_VERSION@.gir: $(INTROSPECTION_SCANNER) Makefile libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la
	$(AM_V_GEN)$(INTROSPECTION_SCANNER) -v \
		--namespace GtkClutter --nsversion=@CLUTTER_GTK_API_VERSION@ \
		--strip-prefix=GtkClutter \
		$(INCLUDES) $(AM_CPPFLAGS) $(common_cflags) \
		--add-include-path=$(srcdir) --add-include=path=. \
		--include=Clutter-1.0 \
		--include=GdkPixbuf-2.0 \
		--include=Gtk-2.0 \
		--library=libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la \
		--libtool="$(top_builddir)/libtool" \
		--output $@ \
		$(sources_h) $(sources_c)

BUILT_GIRSOURCES += GtkClutter-@CLUTTER_GTK_API_VERSION@.gir

# INTROSPECTION_GIRDIR/INTROSPECTION_TYPELIBDIR aren't the right place to
# install anything - we need to install inside our prefix.
girdir = $(datadir)/gir-1.0
gir_DATA = $(BUILT_GIRSOURCES)

typelibsdir = $(libdir)/girepository-1.0/
typelibs_DATA = $(BUILT_GIRSOURCES:.gir=.typelib)

%.typelib: %.gir $(INTROSPECTION_COMPILER)
	$(AM_V_GEN)\
	LD_LIBRARY_PATH=.libs$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH} \
	$(INTROSPECTION_COMPILER) \
		--includedir=$(srcdir) \
		--includedir=. \
		$(INTROSPECTION_COMPILER_OPTS) \
	$< -o $(builddir)/$(@F)

CLEANFILES += $(BUILT_GIRSOURCES) $(typelibs_DATA)
endif
