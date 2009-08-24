/* Clutter-Gtk Window Test
 *
 * (c) 2009, Collabora Ltd.
 *
 * Written by Davyd Madeley <davyd.madeley@collabora.co.uk>
 */

#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>

static GtkWidget *contents = NULL;
static GtkWidget *bin1 = NULL;
static GtkWidget *bin2 = NULL;

static void
reparent_cb (GtkButton *button, gpointer user_data)
{
    /* find out who the current parent is */
    GtkWidget *parent = gtk_widget_get_parent (contents);
    GtkWidget *newparent = NULL;

    if (parent == bin1)
    {
        g_print ("Current parent = LHS\n");
        newparent = bin2;
    }
    else if (parent == bin2)
    {
        g_print ("Current parent = RHS\n");
        newparent = bin1;
    }
    else
    {
        g_print ("Unknown parent: %s (%p)\n",
                G_OBJECT_TYPE_NAME (parent), parent);
        g_return_if_reached ();
    }

    gtk_widget_reparent (contents, newparent);
}

int
main (int argc, char **argv)
{
    gtk_clutter_init (&argc, &argv);

    GtkWidget *window = gtk_clutter_window_new ();
    GtkWidget *table = gtk_table_new (2, 2, FALSE);

    gtk_container_add (GTK_CONTAINER (window), table);

    bin1 = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
    gtk_table_attach_defaults (GTK_TABLE (table), bin1, 0, 1, 0, 1);

    ClutterActor *actor = gtk_clutter_actor_new ();
    bin2 = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (actor));
    GtkWidget *standin = gtk_clutter_standin_new (actor);
    gtk_table_attach_defaults (GTK_TABLE (table), standin, 1, 2, 0, 1);

    GtkWidget *button = gtk_button_new_with_label ("Reparent");
    gtk_table_attach_defaults (GTK_TABLE (table), button, 0, 2, 1, 2);

    contents = gtk_image_new_from_file ("redhand.png");
    gtk_container_add (GTK_CONTAINER (bin1), contents);

    GtkSizeGroup *sizegroup = gtk_size_group_new (GTK_SIZE_GROUP_BOTH);
    gtk_size_group_add_widget (sizegroup, bin1);
    gtk_size_group_add_widget (sizegroup, standin);

    g_signal_connect_swapped (window, "destroy",
            G_CALLBACK (gtk_main_quit), NULL);

    g_signal_connect (button, "clicked",
            G_CALLBACK (reparent_cb), NULL);

    gtk_widget_show_all (window);
    gtk_main ();
}
