/* Clutter-Gtk Window Test
 *
 * (c) 2009, Collabora Ltd.
 *
 * Written by Davyd Madeley <davyd.madeley@collabora.co.uk>
 */

#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>

static void
button_clicked (GtkButton *button, char *stock_id)
{
    g_print ("button clicked: %s\n", stock_id);
}

static gboolean
expose_monitor (GtkWidget *widget, GdkEventExpose *event, char *str)
{
    g_print ("expose (%s) - (%i, %i, %i, %i)\n",
            str,
            event->area.x, event->area.y,
            event->area.width, event->area.height);

    return FALSE;
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

    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_widget_show_all (window);
    gtk_main ();
}
