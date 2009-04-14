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

lib_LTLIBRARIES = libclutter-gtk-0.9.la

# please, keep the list sorted alphabetically
libclutter_gtk_0_9_la_SOURCES = 	\
        gtk-clutter-embed.c 		\
        gtk-clutter-scrollable.c 	\
        gtk-clutter-util.c 		\
        gtk-clutter-viewport.c

libclutter_gtk_0_9_la_LIBADD = $(CLUTTER_LIBS) $(GTK_LIBS)

libclutter_gtk_0_9_la_LDFLAGS = $(CLUTTER_LT_LDFLAGS)

cluttergtkheadersdir = $(includedir)/clutter-0.9/clutter-gtk

# please, keep the list sorted alphabetically
cluttergtkheaders_HEADERS = 		\
        clutter-gtk.h 			\
        gtk-clutter-embed.h 		\
        gtk-clutter-scrollable.h 	\
        gtk-clutter-util.h 		\
        gtk-clutter-viewport.h
