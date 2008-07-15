NULL =

noinst_PROGRAMS = \
	gtk-clutter-test \
	gtk-clutter-events \
	gtk-clutter-multistage

INCLUDES = \
	-I$(srcdir) \
	-I$(top_srcdir)

AM_CPPFLAGS = $(CLUTTER_CFLAGS) $(GTK_CFLAGS)

common_deps = \
	$(top_builddir)/clutter-gtk/libclutter-gtk-0.8.la

common_ldadd = \
	$(top_builddir)/clutter-gtk/libclutter-gtk-0.8.la \
	$(CLUTTER_LIBS)	\
	$(GTK_LIBS)

gtk_clutter_test_SOURCES = gtk-clutter-test.c
gtk_clutter_test_DEPENDENCIES = $(common_deps)
gtk_clutter_test_LDADD = $(common_ldadd)

gtk_clutter_events_SOURCES = gtk-clutter-events.c
gtk_clutter_events_DEPENDENCIES = $(common_deps)
gtk_clutter_events_LDADD = $(common_ldadd)

gtk_clutter_multistage_SOURCES = gtk-clutter-multistage.c
gtk_clutter_multistage_DEPENDENCIES = $(common_deps)
gtk_clutter_multistage_LDADD = $(common_ldadd)

EXTRA_DIST = redhand.png
