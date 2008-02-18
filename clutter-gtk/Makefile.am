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

lib_LTLIBRARIES = libclutter-gtk-0.6.la

libclutter_gtk_0_6_la_SOURCES = gtk-clutter-embed.c
libclutter_gtk_0_6_la_LIBADD = $(CLUTTER_LIBS) $(GTK_LIBS)
libclutter_gtk_0_6_la_LDFLAGS = $(CLUTTER_LT_LDFLAGS)

cluttergtkheadersdir = $(includedir)/clutter-0.6/clutter-gtk
cluttergtkheaders_HEADERS = gtk-clutter-embed.h
