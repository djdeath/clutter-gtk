/* Clutter-Gtk Window Test
 *
 * (c) 2009, Collabora Ltd.
 *
 * Written by Davyd Madeley <davyd.madeley@collabora.co.uk>
 */

#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>

int
main (int argc, char **argv)
{
    gtk_clutter_init (&argc, &argv);

    GtkWidget *window = gtk_clutter_window_new ();
    GtkWidget *table = gtk_table_new (6, 6, TRUE);

    gtk_table_attach_defaults (GTK_TABLE (table),
		    gtk_button_new_from_stock (GTK_STOCK_OK),
		    0, 1, 0, 1);
    gtk_table_attach_defaults (GTK_TABLE (table),
		    gtk_button_new_from_stock (GTK_STOCK_CANCEL),
		    1, 2, 1, 2);
    gtk_table_attach_defaults (GTK_TABLE (table),
		    gtk_button_new_from_stock (GTK_STOCK_CLOSE),
		    2, 3, 2, 3);
    gtk_table_attach_defaults (GTK_TABLE (table),
		    gtk_button_new_from_stock (GTK_STOCK_ABOUT),
		    3, 4, 3, 4);
    gtk_table_attach_defaults (GTK_TABLE (table),
		    gtk_button_new_from_stock (GTK_STOCK_BOLD),
		    4, 5, 4, 5);
    gtk_table_attach_defaults (GTK_TABLE (table),
		    gtk_button_new_from_stock (GTK_STOCK_ITALIC),
		    5, 6, 5, 6);

    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_widget_show_all (window);
    // gtk_widget_show_all (table);
    gtk_main ();
}
