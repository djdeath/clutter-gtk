INCLUDES = \
	-I$(srcdir) 				\
	-I$(top_srcdir) 			\
	-DG_LOG_DOMAIN=\"Clutter-Gtk\" 		\
	-DPREFIX=\""$(prefix)"\" 		\
	-DLIBDIR=\""$(libdir)"\" 		\
	$(CLUTTER_DEBUG_FLAGS) 			\
	$(GCC_FLAGS) 				\
	$(CLUTTER_CFLAGS) 			\
	$(GTK_CFLAGS)

lib_LTLIBRARIES = libclutter-gtk-0.4.la

libclutter_gtk_0_4_la_SOURCES = clutter-gtk.c
libclutter_gtk_0_4_la_LIBADD = $(CLUTTER_LIBS) $(GTK_LIBS)
libclutter_gtk_0_4_la_LDFLAGS = $(CLUTTER_LT_LDFLAGS)

cluttergtkheadersdir = $(includedir)/clutter-0.4/clutter-gtk
cluttergtkheaders_HEADERS = clutter-gtk.h
