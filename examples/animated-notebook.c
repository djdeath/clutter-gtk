/* Animated Notebook Demonstration
 *
 * (c) 2009, Collabora Ltd.
 *
 * Written by Davyd Madeley <davyd.madeley@collabora.co.uk>
 */

#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/clutter-gtk.h>

static void
animation_complete (ClutterTimeline *timeline, ClutterActor *actor)
{
    /* reparent the page into the gtkalignment, because we don't need
     * 1 million actors lying around */
    GtkWidget *bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (actor));
    GtkWidget *page = gtk_bin_get_child (GTK_BIN (bin));
    GtkWidget *parent = GTK_WIDGET (g_object_get_data (G_OBJECT (page),
                "future-parent"));

    // gtk_clutter_actor_set_ignore_damage (GTK_CLUTTER_ACTOR (actor), FALSE);
    gtk_widget_reparent (page, parent);
    clutter_actor_destroy (actor);
}

static void
page_removed (ClutterTimeline *timeline, ClutterActor *actor)
{
    GtkWidget *bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (actor));
    GtkWidget *page = gtk_bin_get_child (GTK_BIN (bin));

    /* remove this page from the bin so it doesn't get disposed */
    gtk_container_remove (GTK_CONTAINER (bin), page);
    clutter_actor_destroy (actor);
}

static void
page_changed (GtkTreeSelection *selection, GtkBuilder *ui)
{
    /* the page has probably been changed */

    GtkTreeModel *model;
    GtkTreeIter iter;
    gtk_tree_selection_get_selected (selection, &model, &iter);

    GtkWidget *page;
    gtk_tree_model_get (model, &iter,
            2, &page,
            -1);

    g_return_if_fail (GTK_IS_WIDGET (page));

    /* get the event box */
    GtkWidget *bin = GTK_WIDGET (gtk_builder_get_object (ui, "page-box"));

    if (gtk_bin_get_child (GTK_BIN (bin)) == page)
    {
        g_object_unref (page);
        return;
    }

    /* get the ClutterStage */
    GtkWidget *treeview = GTK_WIDGET (gtk_tree_selection_get_tree_view (selection));
    GtkWidget *toplevel = gtk_widget_get_toplevel (treeview);

    g_return_if_fail (GTK_CLUTTER_IS_WINDOW (toplevel));

    ClutterActor *stage = gtk_clutter_window_get_stage (
            GTK_CLUTTER_WINDOW (toplevel));

    /* unparent the old child into an actor */
    ClutterActor *actor = gtk_clutter_actor_new ();
    gtk_widget_reparent (gtk_bin_get_child (GTK_BIN (bin)),
            gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (actor)));

    GtkAllocation allocation;
    gtk_widget_get_allocation (bin, &allocation);

    clutter_container_add_actor (CLUTTER_CONTAINER (stage), actor);
    ClutterAnimation *animation = clutter_actor_animate (actor,
            CLUTTER_EASE_OUT_SINE, 500,
            "fixed::x", (float) allocation.x,
            "fixed::y", (float) allocation.y,
            "fixed::scale-gravity", CLUTTER_GRAVITY_CENTER,
            "scale-x", 0.,
            "scale-y", 0.,
            "opacity", 0x0,
            NULL);

    g_signal_connect_after (clutter_animation_get_timeline (animation),
            "completed",
            G_CALLBACK (page_removed),
            actor);

    /* put the page onto a GtkClutterActor */
    actor = gtk_clutter_actor_new ();
    gtk_container_add (GTK_CONTAINER (gtk_clutter_actor_get_widget (
                    GTK_CLUTTER_ACTOR (actor))), page);
    g_object_unref (page);

    clutter_container_add_actor (CLUTTER_CONTAINER (stage), actor);

    /* get the initial position */
    GdkRectangle rect;
    GtkTreeViewColumn *column = gtk_tree_view_get_column (
            GTK_TREE_VIEW (treeview), 0);
    GtkTreePath *path = gtk_tree_model_get_path (model, &iter);
    gtk_tree_view_get_cell_area (GTK_TREE_VIEW (treeview), path, column,
            &rect);
    gtk_tree_path_free (path);

    gtk_tree_view_convert_tree_to_widget_coords (GTK_TREE_VIEW (treeview),
            rect.x + rect.width,
            rect.y,
            &rect.x,
            &rect.y);

    g_object_set_data (G_OBJECT (page), "future-parent", bin);

    /* animate the actor to expand from the initial position to the final
     * position (the allocation of the GtkEventBox) */
    animation = clutter_actor_animate (actor,
            CLUTTER_EASE_IN_SINE, 700,
            "fixed::x", (float) rect.x,
            "fixed::y", (float) rect.y,
            "fixed::scale-x", 0.,
            "fixed::scale-y", (float) rect.height / (float) allocation.height,
            "fixed::scale-gravity", CLUTTER_GRAVITY_NORTH_WEST,
            "x", (float) allocation.x,
            "y", (float) allocation.y,
            "scale-x", 1.,
            "scale-y", 1.,
            NULL);

    g_signal_connect_after (clutter_animation_get_timeline (animation),
            "completed",
            G_CALLBACK (animation_complete),
            actor);
}

int
main (int argc, char **argv)
{
    GError *error = NULL;

    gtk_clutter_init (&argc, &argv);

    GtkBuilder *ui = gtk_builder_new ();
    gtk_builder_add_from_file (ui, "animated-notebook.ui", &error);
    if (error) g_error ("%s", error->message);

    /* iterate the model, load each of the pages */
    GtkTreeModel *model = GTK_TREE_MODEL (gtk_builder_get_object (ui,
                "pages-store"));
    GtkTreeIter iter;
    gboolean valid;
    for (valid = gtk_tree_model_get_iter_first (model, &iter);
         valid;
         valid = gtk_tree_model_iter_next (model, &iter))
    {
        char *ui_name;
        gtk_tree_model_get (model, &iter,
                1, &ui_name,
                -1);

        g_print ("Looking up %s\n", ui_name);

        GtkWidget *page = GTK_WIDGET (gtk_builder_get_object (ui, ui_name));
        gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                2, page,
                -1);

        g_object_weak_ref (G_OBJECT (page), (GWeakNotify) g_print,
                g_strdup_printf ("Finalised %s (%%p)\n", ui_name));

        g_free (ui_name);
    }

    /* construct the toplevel UI */
    GtkWidget *window = gtk_clutter_window_new ();
    GtkWidget *hbox = GTK_WIDGET (gtk_builder_get_object (ui,
                "main-window-toplevel"));
    gtk_container_add (GTK_CONTAINER (window), hbox);

    /* Glade doesn't expose GtkTreeSelection */
    GtkWidget *treeview = GTK_WIDGET (gtk_builder_get_object (ui,
                "page-treeview"));
    g_signal_connect (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
            "changed",
            G_CALLBACK (page_changed),
            ui);

    gtk_widget_show_all (window);
    gtk_main ();

    return 0;
}
