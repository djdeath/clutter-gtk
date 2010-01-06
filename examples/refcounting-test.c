/* Clutter-Gtk Window Test
 *
 * (c) 2009, Collabora Ltd.
 *
 * Written by Davyd Madeley <davyd.madeley@collabora.co.uk>
 */

#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>

#include <math.h>

static ClutterActor *container = NULL;
static ClutterActor *actor = NULL;
static GtkWidget *bin = NULL;
static GtkWidget *image = NULL;

static void
object_finalized (gpointer ptr, GObject *obj)
{
    g_print ("Object finalized: %p\n", obj);

    *((GObject **)ptr) = NULL;
}

#define GET_REF_COUNT(p) (((p) != NULL) ? G_OBJECT ((p))->ref_count : -1)

static void
add_actor_cb (GtkToggleButton *toggle, gpointer user_data)
{
    if (gtk_toggle_button_get_active (toggle))
    {
        /* create children */
        actor = gtk_clutter_actor_new ();
        bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (actor));
        image = gtk_image_new_from_file ("redhand.png");

        clutter_container_add_actor (CLUTTER_CONTAINER (container), actor);
        gtk_container_add (GTK_CONTAINER (bin), image);
        gtk_widget_show (image);

        g_object_weak_ref (G_OBJECT (actor), object_finalized, &actor);
        g_object_weak_ref (G_OBJECT (bin), object_finalized, &bin);
        g_object_weak_ref (G_OBJECT (image), object_finalized, &image);
    }
    else
    {
        /* destroy children */
        g_print ("Removing image from parent\n");
        g_object_ref (image);
        gtk_container_remove (GTK_CONTAINER (bin), image);
        g_print ("Destroying actor\n");
        clutter_actor_destroy (actor);
    }

    g_print ("\nRef Counts\n");
    g_print ("----------\n");
    g_print ("GtkClutterActor     (%p): %i\n", actor, GET_REF_COUNT (actor));
    g_print ("GtkClutterOffscreen (%p): %i\n", bin, GET_REF_COUNT (bin));
    g_print ("GtkImage            (%p): %i\n", image, GET_REF_COUNT (image));
}

int
main (int argc, char **argv)
{
    gtk_clutter_init (&argc, &argv);

    GtkWidget *window = gtk_clutter_window_new ();
    GtkWidget *vbox = gtk_vbox_new (FALSE, 6);

    gtk_container_add (GTK_CONTAINER (window), vbox);

    container = clutter_group_new ();

    GtkWidget *standin = gtk_clutter_standin_new (container);
    gtk_box_pack_start (GTK_BOX (vbox), standin, TRUE, TRUE, 0);

    GtkWidget *toggle = gtk_toggle_button_new_with_label ("Add actor");
    gtk_box_pack_start (GTK_BOX (vbox), toggle, FALSE, TRUE, 0);

    g_signal_connect_swapped (window, "destroy",
            G_CALLBACK (gtk_main_quit), NULL);

    g_signal_connect (toggle, "toggled",
            G_CALLBACK (add_actor_cb), NULL);

    gtk_widget_show_all (window);
    gtk_main ();

    return 0;
}
