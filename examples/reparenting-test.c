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

static gulong CLUTTER_COS_SQUARED = CLUTTER_ANIMATION_LAST;

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

static gdouble
clutter_cos_squared (ClutterAlpha *alpha, gpointer user_data)
{
    ClutterTimeline *timeline = clutter_alpha_get_timeline (alpha);
    gdouble p = clutter_timeline_get_progress (timeline);

    return pow (cos (M_PI * p), 2);
}

static void
animate_cb (GtkToggleButton *toggle, ClutterTimeline *timeline)
{
    if (gtk_toggle_button_get_active (toggle))
    {
        clutter_timeline_start (timeline);
    }
    else
    {
        clutter_timeline_pause (timeline);
    }
}

int
main (int argc, char **argv)
{
    gtk_clutter_init (&argc, &argv);

    /* register the alpha func */
    CLUTTER_COS_SQUARED = clutter_alpha_register_func (clutter_cos_squared, NULL);

    GtkWidget *window = gtk_clutter_window_new ();
    GtkWidget *table = gtk_table_new (2, 3, FALSE);

    gtk_container_add (GTK_CONTAINER (window), table);

    bin1 = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
    gtk_table_attach_defaults (GTK_TABLE (table), bin1, 0, 1, 0, 1);

    ClutterActor *actor = gtk_clutter_actor_new ();
    bin2 = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (actor));
    GtkWidget *standin = gtk_clutter_standin_new (actor);
    gtk_table_attach_defaults (GTK_TABLE (table), standin, 1, 2, 0, 1);

    GtkWidget *button = gtk_button_new_with_label ("Reparent");
    gtk_table_attach_defaults (GTK_TABLE (table), button, 0, 2, 1, 2);

    GtkWidget *toggle = gtk_toggle_button_new_with_label ("Animate RHS");
    gtk_table_attach_defaults (GTK_TABLE (table), toggle, 0, 2, 2, 3);

    contents = gtk_image_new_from_file ("redhand.png");
    gtk_container_add (GTK_CONTAINER (bin1), contents);

    /* put both sides of the table into a sizegroup to avoid resizing */
    GtkSizeGroup *sizegroup = gtk_size_group_new (GTK_SIZE_GROUP_BOTH);
    gtk_size_group_add_widget (sizegroup, bin1);
    gtk_size_group_add_widget (sizegroup, standin);

    /* add an optional animation to the actor */
    ClutterAnimation *animation = clutter_actor_animate (actor,
            CLUTTER_COS_SQUARED, 1200,
            "opacity", 0x0,
            NULL);
    ClutterTimeline *timeline = clutter_animation_get_timeline (animation);
    clutter_timeline_set_loop (timeline, TRUE);
    clutter_timeline_stop (timeline);

    g_signal_connect_swapped (window, "destroy",
            G_CALLBACK (gtk_main_quit), NULL);

    g_signal_connect (button, "clicked",
            G_CALLBACK (reparent_cb), NULL);

    g_signal_connect (toggle, "toggled",
            G_CALLBACK (animate_cb), timeline);

    gtk_widget_show_all (window);
    gtk_main ();
}
