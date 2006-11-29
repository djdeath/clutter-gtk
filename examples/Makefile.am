noinst_PROGRAMS = gtk-clutter-test

INCLUDES = \
	-I$(srcdir) \
	-I$(top_srcdir) \
	$(CLUTTER_CFLAGS) \
	$(GTK_CFLAGS)

gtk_clutter_test_DEPENDENCIES = $(top_srcdir)/clutter-gtk/libcluttergtk-1.0.la
gtk_clutter_test_SOURCES = gtk-clutter-test.c
gtk_clutter_test_LDADD = \
	$(top_srcdir)/clutter-gtk/libcluttergtk-1.0.la \
	$(CLUTTER_LIBS)	\
	$(GTK_LIBS)
