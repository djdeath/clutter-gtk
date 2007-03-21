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

lib_LTLIBRARIES = libcluttergtk-1.0.la

libcluttergtk_1_0_la_SOURCES = gtk-clutter.c
libcluttergtk_1_0_la_LIBADD = $(CLUTTER_LIBS) $(GTK_LIBS)
libcluttergtk_1_0_la_LDFLAGS = $(CLUTTER_LT_LDFLAGS)

cluttergtkheadersdir = $(includedir)/clutter-0.2/clutter-gtk
cluttergtkheaders_HEADERS = gtk-clutter.h
