/* Clutter-Gtk Test
 *
 * (c) 2009, Collabora Ltd.
 *
 * Written by Danielle Madeley <danielle.madeley@collabora.co.uk>
 */

#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>

#include <math.h>

static void
toggle_animations (GtkToggleButton *toggle, ClutterTimeline *timeline)
{
    if (gtk_toggle_button_get_active (toggle))
    {
        /* run the animations */
        clutter_timeline_start (timeline);
    }
    else
    {
        /* stop the animations */
        clutter_timeline_stop (timeline);
    }
}

int
main (int argc, char **argv)
{
    ClutterActor *actor;
    GtkWidget *standin;
    ClutterAnimation *animation;
    GtkWidget *toggle;
    ClutterTimeline *nb_tl, *button_tl, *image_tl;

    gtk_clutter_init (&argc, &argv);

    GtkWidget *window = gtk_clutter_window_new ();
    g_signal_connect_swapped (window, "show",
            G_CALLBACK (g_print), "show window (%p)\n");
    g_signal_connect_swapped (window, "destroy",
            G_CALLBACK (gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_vbox_new (FALSE, 6);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_container_set_border_width (GTK_CONTAINER (window), 6);

    /* notebook */
    GtkWidget *nb = gtk_notebook_new ();
    g_signal_connect_swapped (nb, "show",
            G_CALLBACK (g_print), "show notebook (%p)\n");
    actor = gtk_clutter_actor_new_with_contents (nb);
    g_signal_connect_swapped (actor, "show",
            G_CALLBACK (g_print), "show notebook actor (%p)\n");
    standin = gtk_clutter_standin_new (actor);
    g_signal_connect_swapped (standin, "show",
            G_CALLBACK (g_print), "show notebook standin (%p)\n");

    animation = clutter_actor_animate (actor, CLUTTER_LINEAR, 800,
            "fixed::rotation-angle-z", 0.,
            "rotation-angle-z", 360.,
            "fixed::rotation-center-z-gravity", CLUTTER_GRAVITY_CENTER,
            NULL);
    clutter_animation_set_loop (animation, TRUE);
    nb_tl = clutter_animation_get_timeline (animation);
    clutter_timeline_stop (nb_tl);

    gtk_box_pack_start (GTK_BOX (vbox), standin, TRUE, TRUE, 0);

    /* button */
    GtkWidget *button = gtk_button_new ();
    g_signal_connect_swapped (button, "show",
            G_CALLBACK (g_print), "show button (%p)\n");
    g_signal_connect_swapped (button, "clicked",
           G_CALLBACK (g_print), "clicked\n");

    actor = gtk_clutter_actor_new_with_contents (button);
    g_signal_connect_swapped (actor, "show",
            G_CALLBACK (g_print), "show button actor (%p\n");
    standin = gtk_clutter_standin_new (actor);
    g_signal_connect_swapped (standin, "show",
            G_CALLBACK (g_print), "show button standin (%p)\n");

    animation = clutter_actor_animate (actor, CLUTTER_LINEAR, 800,
            "fixed::rotation-angle-z", 360.,
            "rotation-angle-z", 0.,
            "fixed::rotation-center-z-gravity", CLUTTER_GRAVITY_CENTER,
            NULL);
    clutter_animation_set_loop (animation, TRUE);
    button_tl = clutter_animation_get_timeline (animation);
    clutter_timeline_stop (button_tl);

    gtk_notebook_append_page (GTK_NOTEBOOK (nb),
            standin, gtk_label_new ("Standins"));

    /* image */
    GtkWidget *image = gtk_image_new_from_file ("redhand.png");
    g_signal_connect_swapped (actor, "show",
            G_CALLBACK (g_print), "show image (%p\n");

    actor = gtk_clutter_actor_new_with_contents (image);
    g_signal_connect_swapped (actor, "show",
            G_CALLBACK (g_print), "show image actor (%p\n");
    standin = gtk_clutter_standin_new (actor);
    g_signal_connect_swapped (actor, "show",
            G_CALLBACK (g_print), "show image standin (%p\n");

    animation = clutter_actor_animate (actor, CLUTTER_LINEAR, 800,
            "fixed::rotation-angle-z", 0.,
            "rotation-angle-z", 360.,
            "fixed::rotation-center-z-gravity", CLUTTER_GRAVITY_CENTER,
            NULL);
    clutter_animation_set_loop (animation, TRUE);
    image_tl = clutter_animation_get_timeline (animation);
    clutter_timeline_stop (image_tl);

    gtk_container_add (GTK_CONTAINER (button), standin);

    /* Page 2 */
    button = gtk_button_new ();
    g_signal_connect_swapped (button, "clicked",
           G_CALLBACK (g_print), "clicked\n");

    gtk_notebook_append_page (GTK_NOTEBOOK (nb),
            button, gtk_label_new ("No Standins"));

    image = gtk_image_new_from_file ("redhand.png");
    gtk_container_add (GTK_CONTAINER (button), image);

    /* toggles */
    toggle = gtk_toggle_button_new_with_label ("Animate Notebook");
    gtk_box_pack_start (GTK_BOX (vbox), toggle, FALSE, TRUE, 0);
    g_signal_connect (toggle, "toggled",
            G_CALLBACK (toggle_animations), nb_tl);

    toggle = gtk_toggle_button_new_with_label ("Animate Button");
    gtk_box_pack_start (GTK_BOX (vbox), toggle, FALSE, TRUE, 0);
    g_signal_connect (toggle, "toggled",
            G_CALLBACK (toggle_animations), button_tl);

    toggle = gtk_toggle_button_new_with_label ("Animate Image");
    gtk_box_pack_start (GTK_BOX (vbox), toggle, FALSE, TRUE, 0);
    g_signal_connect (toggle, "toggled",
            G_CALLBACK (toggle_animations), image_tl);

    g_print (" --- show all ---\n");
    gtk_widget_show_all (window);
    g_print (" --- main ---\n");
    gtk_main ();
}
