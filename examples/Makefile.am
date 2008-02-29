noinst_PROGRAMS = gtk-clutter-test gtk-clutter-events

INCLUDES = \
	-I$(srcdir) \
	-I$(top_srcdir) \
	$(CLUTTER_CFLAGS) \
	$(GTK_CFLAGS)

gtk_clutter_test_DEPENDENCIES = \
	$(top_builddir)/clutter-gtk/libclutter-gtk-0.7.la
gtk_clutter_test_SOURCES = gtk-clutter-test.c
gtk_clutter_test_LDADD = \
	$(top_builddir)/clutter-gtk/libclutter-gtk-0.7.la \
	$(CLUTTER_LIBS)	\
	$(GTK_LIBS)

gtk_clutter_events_DEPENDENCIES = \
	$(top_builddir)/clutter-gtk/libclutter-gtk-0.7.la
gtk_clutter_events_SOURCES = gtk-clutter-events.c
gtk_clutter_events_LDADD = \
	$(top_builddir)/clutter-gtk/libclutter-gtk-0.7.la \
	$(CLUTTER_LIBS)	\
	$(GTK_LIBS)

EXTRA_DIST = redhand.png
