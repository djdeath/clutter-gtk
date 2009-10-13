/* 
 * Clutter-Gtk GtkScrolledWindow Test
 *
 * (c) 2009, Collabora Ltd.
 *
 * Authors:
 *     Danielle Madeley <danielle.madeley@collabora.co.uk>
 */

#include <clutter-gtk/clutter-gtk.h>

int
main (int argc, char **argv)
{
    gtk_clutter_init (&argc, &argv);

    GtkWidget *window = gtk_clutter_window_new ();
    GtkWidget *vbox = gtk_vbox_new (TRUE, 0);

    ClutterActor *viewport = gtk_clutter_viewport_new (NULL, NULL, NULL);
    GtkAdjustment *hadj, *vadj;
    g_object_get (viewport,
            "hadjustment", &hadj,
            "vadjustment", &vadj,
            NULL);

    GtkWidget *sw = gtk_scrolled_window_new (hadj, vadj);
    g_object_unref (hadj);
    g_object_unref (vadj);

    ClutterActor *text = clutter_text_new_with_text ("Sans 18",
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
            "Fusce vulputate nisi et purus vestibulum fermentum. Fusce "
            "sed metus eu augue dapibus egestas. Proin tincidunt lectus "
            "in dolor posuere tincidunt. Integer eu est metus, a luctus "
            "metus. Aenean sed nulla in nulla laoreet sodales a id erat. "
            "Pellentesque sodales augue non lectus dictum mollis. Proin "
            "imperdiet, lorem id pharetra dignissim, lacus nunc condimentum "
            "massa, sit amet varius justo elit et felis. Quisque sagittis "
            "tellus a ante vehicula gravida. Vivamus sit amet magna ante. "
            "Ut id aliquet diam.");
    clutter_container_add_actor (CLUTTER_CONTAINER (viewport), text);

    GtkWidget *standin = gtk_clutter_standin_new (viewport);
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (sw), standin);

    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_box_pack_start_defaults (GTK_BOX (vbox), sw);

    gtk_widget_show_all (window);
    gtk_main ();
}
