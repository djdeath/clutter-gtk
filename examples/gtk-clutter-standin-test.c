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

static GtkWidget *
add_button (GtkTable *table, char *stock_id, int row)
{
    GtkWidget *button = gtk_button_new_from_stock (stock_id);
    gtk_table_attach_defaults (GTK_TABLE (table), button,
            row, row + 1, row, row + 1);

    g_signal_connect (button, "clicked",
            G_CALLBACK (button_clicked), stock_id);

    return button;
}

int
main (int argc, char **argv)
{
    gtk_clutter_init (&argc, &argv);

    GtkWidget *window = gtk_clutter_window_new ();
    GtkWidget *table = gtk_table_new (6, 6, FALSE);

    add_button (GTK_TABLE (table), GTK_STOCK_OK, 0);
    add_button (GTK_TABLE (table), GTK_STOCK_CANCEL, 1);
    add_button (GTK_TABLE (table), GTK_STOCK_CLOSE, 2);

    ClutterActor *texture = clutter_texture_new_from_file ("redhand.png", NULL);
    GtkWidget *standin = gtk_clutter_standin_new (texture);
    gtk_table_attach_defaults (GTK_TABLE (table), standin, 3, 4, 3, 4);

    add_button (GTK_TABLE (table), GTK_STOCK_BOLD, 4);
    add_button (GTK_TABLE (table), GTK_STOCK_ITALIC, 5);

    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_widget_show_all (window);

    /* add a rotation to our actor */
    ClutterAnimation *animation = clutter_actor_animate (texture,
            CLUTTER_LINEAR, 800,
            "rotation-angle-y", 360.,
            NULL);
    clutter_animation_set_loop (animation, TRUE);

    g_signal_connect_swapped (window, "destroy",
            G_CALLBACK (gtk_main_quit), NULL);

    gtk_main ();

    return 0;
}
