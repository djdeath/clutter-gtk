/* Clutter-Gtk Window Test
 *
 * (c) 2009, Collabora Ltd.
 *
 * Written by Davyd Madeley <davyd.madeley@collabora.co.uk>
 */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>

static void
animation_stage_2_complete (ClutterTimeline *timeline, ClutterActor *texture)
{
    /* undo our changes */
    GtkWidget *image = GTK_WIDGET (g_object_get_data (G_OBJECT (texture),
                "image"));
    char *stock_id = g_object_get_data (G_OBJECT (texture), "stock-id");
    GtkIconSize size = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (texture), "icon-size"));

    g_print ("set stock = %s, size = %i\n", stock_id, size);
    gtk_image_set_from_stock (GTK_IMAGE (image), stock_id, size);
    clutter_actor_destroy (texture);
}

static void
animation_stage_1_complete (ClutterTimeline *timeline, ClutterActor *texture)
{
    GtkAllocation allocation;
    GtkWidget *image;
    ClutterAnimation *animation;

    image = GTK_WIDGET (g_object_get_data (G_OBJECT (texture), "image"));
    gtk_widget_get_allocation (image, &allocation);

    /* do the second animation, have the icon grow out from the middle of the
     * button */
    animation = clutter_actor_animate (texture, CLUTTER_EASE_OUT_SINE, 100,
                                       "fixed::x", (float) allocation.x,
                                       "fixed::y", (float) allocation.y,
                                       "fixed::scale-x", 0.,
                                       "fixed::scale-y", 0.,
                                       "scale-x", 1.,
                                       "scale-y", 1.,
                                       "fixed::scale-gravity", CLUTTER_GRAVITY_CENTER,
                                       NULL);

    g_signal_connect_after (clutter_animation_get_timeline (animation),
                            "completed",
                            G_CALLBACK (animation_stage_2_complete),
                            texture);
}

static void
button_clicked (GtkButton *button, char *stock_id)
{
    GtkWidget *toplevel;

    g_print ("button clicked: %s\n", stock_id);

    /* this whole function is really, really hacky and should never, ever
     * be copied into a real program. It's just a demonstration of what
     * is hypothetically possible with GTK+ CSW and Clutter */

    toplevel = gtk_widget_get_toplevel (GTK_WIDGET (button));
    if (GTK_CLUTTER_IS_WINDOW (toplevel))
    {
        ClutterActor *texture, *stage;
        GtkWidget *image;
        char *icon_name;
        GtkAllocation allocation;
        GtkAllocation toplevel_allocation;
        GtkIconSize size;
        GdkPixbuf *blank;
        ClutterAnimation *animation;

        /* create a texture from the button image */
        texture = gtk_clutter_texture_new ();
        image = gtk_button_get_image (button);
        gtk_image_get_stock (GTK_IMAGE (image), &icon_name, &size);
        gtk_clutter_texture_set_from_stock (GTK_CLUTTER_TEXTURE (texture),
                                            GTK_WIDGET (button),
                                            icon_name, size, NULL);

        /* position the texture on top of the existing icon */
        stage = gtk_clutter_window_get_stage (GTK_CLUTTER_WINDOW (toplevel));
        clutter_container_add_actor (CLUTTER_CONTAINER (stage), texture);

        g_object_set_data (G_OBJECT (texture), "image", image);
        g_object_set_data_full (G_OBJECT (texture), "stock-id",
                                g_strdup (icon_name),
                                (GDestroyNotify) g_free);
        g_object_set_data (G_OBJECT (texture), "icon-size",
                           GINT_TO_POINTER (size));

        gtk_widget_get_allocation (image, &allocation);

        /* replace the icon itself */
        blank = gdk_pixbuf_new (GDK_COLORSPACE_RGB,
                                TRUE, 8,
                                allocation.width,
                                allocation.height);
        gdk_pixbuf_fill (blank, 0x00000000);
        gtk_image_set_from_pixbuf (GTK_IMAGE (image), blank);
        g_object_unref (blank);

        /* animate a fall due to gravity */
        gtk_widget_get_allocation (toplevel, &toplevel_allocation);
        animation = clutter_actor_animate (texture, CLUTTER_EASE_IN_QUAD, 200,
                                           "fixed::x", (float) allocation.x,
                                           "fixed::y", (float) allocation.y,
                                           "y", (float) toplevel_allocation.height,
                                           NULL);

        g_signal_connect_after (clutter_animation_get_timeline (animation),
                                "completed",
                                G_CALLBACK (animation_stage_1_complete),
                                texture);
    }
}

static GtkWidget *
add_button (GtkGrid *table, char *stock_id, int row)
{
    GtkWidget *button = gtk_button_new_from_stock (stock_id);

    gtk_grid_insert_row (GTK_GRID (table), row);
    gtk_grid_insert_column (GTK_GRID (table), row);
    gtk_grid_attach (GTK_GRID (table), button, row, row, 1, 1);

    g_signal_connect (button, "clicked",
                      G_CALLBACK (button_clicked),
                      stock_id);

    return button;
}

int
main (int argc, char **argv)
{
    GtkWidget *window, *table;
    GtkSettings *settings;

    if (gtk_clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
      return EXIT_FAILURE;

    window = gtk_clutter_window_new ();

    table = gtk_grid_new ();
    gtk_widget_set_hexpand (table, TRUE);
    gtk_widget_set_vexpand (table, TRUE);

    add_button (GTK_GRID (table), GTK_STOCK_OK, 0);
    add_button (GTK_GRID (table), GTK_STOCK_CANCEL, 1);
    add_button (GTK_GRID (table), GTK_STOCK_CLOSE, 2);
    add_button (GTK_GRID (table), GTK_STOCK_ABOUT, 3);
    add_button (GTK_GRID (table), GTK_STOCK_BOLD, 4);
    add_button (GTK_GRID (table), GTK_STOCK_ITALIC, 5);

    gtk_container_add (GTK_CONTAINER (window), table);
    gtk_widget_show_all (window);

    /* override the gtk-button-images setting, since we kind
     * of rely on this to be TRUE to actually show the stock
     * icon falling off the button
     */
    settings = gtk_widget_get_settings (window);
    g_object_set (settings, "gtk-button-images", TRUE, NULL);

    g_signal_connect (window, "destroy",
                      G_CALLBACK (gtk_main_quit),
                      NULL);

    gtk_main ();

    return EXIT_SUCCESS;
}
