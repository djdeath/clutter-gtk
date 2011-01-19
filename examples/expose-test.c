/* Clutter-Gtk Window Test
 *
 * (c) 2009, Collabora Ltd.
 *
 * Written by Davyd Madeley <davyd.madeley@collabora.co.uk>
 */

#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>

#include <sys/time.h>

static void inline
print_time (void)
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    g_print ("[%ld.%.6ld] ", tv.tv_sec, tv.tv_usec);
}

static void
button_clicked (GtkButton *button, char *stock_id)
{
    print_time ();
    g_print ("button clicked: %s\n", stock_id);
}

static gboolean
expose_monitor (GtkWidget *widget, GdkEventExpose *event, char *str)
{
    print_time ();
    g_print ("expose (%s) - (%i, %i, %i, %i)\n",
            str,
            event->area.x, event->area.y,
            event->area.width, event->area.height);

    return FALSE;
}

static gboolean
damage_monitor (GtkWidget *widget, GdkEventExpose *event, char *str)
{
    print_time ();
    g_print ("damage (%s) - (%i, %i, %i, %i)\n",
            str,
            event->area.x, event->area.y,
            event->area.width, event->area.height);

    return FALSE;
}

static gboolean
press_monitor (GtkWidget *widget, GdkEventExpose *event, char *str)
{
    print_time ();
    g_print ("press (%s)\n", str);

    return FALSE;
}

static gboolean
release_monitor (GtkWidget *widget, GdkEventExpose *event, char *str)
{
    print_time ();
    g_print ("release (%s)\n", str);

    return FALSE;
}

static gboolean
enter_monitor (GtkWidget *widget, GdkEventCrossing *event, char *str)
{
    print_time ();
    g_print ("enter (%s)\n", str);

    return FALSE;
}

static gboolean
leave_monitor (GtkWidget *widget, GdkEventCrossing *event, char *str)
{
    print_time ();
    g_print ("leave (%s)\n", str);

    return FALSE;
}

static gboolean
focus_monitor (GtkWidget *widget, GdkEventFocus *event, char *str)
{
    print_time ();
    g_print ("focus (%s)\n", str);

    return FALSE;
}


static void
paint_monitor (ClutterActor *actor, char *str)
{
    print_time ();
    g_print ("paint (%s)\n", str);
}

static GtkWidget *
add_button (GtkTable *table, char *stock_id, int row)
{
    GtkWidget *button = gtk_button_new_from_stock (stock_id);
    gtk_table_attach_defaults (GTK_TABLE (table), button,
            row, row + 1, row, row + 1);

    g_signal_connect (button, "clicked",
            G_CALLBACK (button_clicked), stock_id);

    g_signal_connect (button, "expose-event",
            G_CALLBACK (expose_monitor), stock_id);
    g_signal_connect (button, "button-press-event",
            G_CALLBACK (press_monitor), stock_id);
    g_signal_connect (button, "button-release-event",
            G_CALLBACK (release_monitor), stock_id);
    g_signal_connect (button, "enter-notify-event",
            G_CALLBACK (enter_monitor), stock_id);
    g_signal_connect (button, "leave-notify-event",
            G_CALLBACK (leave_monitor), stock_id);
    g_signal_connect (button, "focus-in-event",
            G_CALLBACK (focus_monitor), stock_id);
    g_signal_connect (button, "focus-out-event",
            G_CALLBACK (focus_monitor), stock_id);

    return button;
}

int
main (int argc, char **argv)
{
    gtk_clutter_init (&argc, &argv);

    GtkWidget *window = gtk_clutter_window_new ();
    GtkWidget *table = gtk_table_new (6, 6, TRUE);

    add_button (GTK_TABLE (table), GTK_STOCK_OK, 0);
    add_button (GTK_TABLE (table), GTK_STOCK_CANCEL, 1);
    add_button (GTK_TABLE (table), GTK_STOCK_CLOSE, 2);
    add_button (GTK_TABLE (table), GTK_STOCK_ABOUT, 3);
    add_button (GTK_TABLE (table), GTK_STOCK_BOLD, 4);
    add_button (GTK_TABLE (table), GTK_STOCK_ITALIC, 5);

    g_signal_connect (window, "expose-event",
            G_CALLBACK (expose_monitor), "window");
    g_signal_connect (table, "expose-event",
            G_CALLBACK (expose_monitor), "table");

    ClutterActor *stage = gtk_clutter_window_get_stage (GTK_CLUTTER_WINDOW (window));
    ClutterActor *actor = clutter_group_get_nth_child (CLUTTER_GROUP (stage), 0);
    GtkWidget *offscreen = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (actor));

    g_signal_connect (stage, "paint",
            G_CALLBACK (paint_monitor), "stage");
    g_signal_connect (actor, "paint",
            G_CALLBACK (paint_monitor), "actor");
    g_signal_connect (offscreen, "expose-event",
            G_CALLBACK (expose_monitor), "offscreen");
    g_signal_connect (offscreen, "damage-event",
            G_CALLBACK (damage_monitor), "offscreen");

    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_widget_show_all (window);
    gtk_main ();

    return 0;
}
