include $(top_srcdir)/build/autotools/Makefile.am.silent

NULL =

noinst_PROGRAMS = \
	gtk-clutter-events \
	gtk-clutter-multistage \
	gtk-clutter-test \
	gtk-clutter-test-actor \
        gtk-clutter-viewport \
	gtk-clutter-window-test

INCLUDES = -I$(srcdir) -I$(top_srcdir)

AM_CFLAGS = $(MAINTAINER_CFLAGS) $(CLUTTER_CFLAGS) $(GTK_CFLAGS)

common_deps = \
	$(top_builddir)/clutter-gtk/libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la

common_ldadd = \
	$(top_builddir)/clutter-gtk/libclutter-gtk-@CLUTTER_GTK_API_VERSION@.la \
	$(CLUTTER_LIBS)	\
	$(GTK_LIBS)

gtk_clutter_test_SOURCES = gtk-clutter-test.c
gtk_clutter_test_DEPENDENCIES = $(common_deps)
gtk_clutter_test_LDADD = $(common_ldadd)

gtk_clutter_test_actor_SOURCES = gtk-clutter-test-actor.c
gtk_clutter_test_actor_DEPENDENCIES = $(common_deps)
gtk_clutter_test_actor_LDADD = $(common_ldadd)

gtk_clutter_events_SOURCES = gtk-clutter-events.c
gtk_clutter_events_DEPENDENCIES = $(common_deps)
gtk_clutter_events_LDADD = $(common_ldadd)

gtk_clutter_multistage_SOURCES = gtk-clutter-multistage.c
gtk_clutter_multistage_DEPENDENCIES = $(common_deps)
gtk_clutter_multistage_LDADD = $(common_ldadd)

gtk_clutter_viewport_SOURCES = gtk-clutter-viewport.c
gtk_clutter_viewport_DEPENDENCIES = $(common_deps)
gtk_clutter_viewport_LDADD = $(common_ldadd)

gtk_clutter_window_test_SOURCES = gtk-clutter-window-test.c
gtk_clutter_window_test_DEPENDENCIES = $(common_deps)
gtk_clutter_window_test_LDADD = $(common_ldadd)

EXTRA_DIST = redhand.png
