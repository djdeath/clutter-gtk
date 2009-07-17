#include <gtk/gtk.h>
#include <clutter/clutter.h>

#include <clutter-gtk/clutter-gtk.h>

static void
on_stage2_allocation_changed (ClutterActor           *stage_2,
                              const ClutterActorBox  *allocation,
                              ClutterAllocationFlags  flags,
                              ClutterActor           *texture_2)
{
  clutter_actor_set_position (texture_2,
                              (allocation->x2 - allocation->x1) / 2,
                              (allocation->y2 - allocation->y1) / 2);
}

int
main (int argc, char *argv[])
{
  ClutterActor *stage0, *stage1, *stage2, *tex1, *tex2;
  GtkWidget *window, *clutter0, *clutter1, *clutter2;
  GtkWidget *notebook, *vbox;
  ClutterColor col0 = { 0xdd, 0xff, 0xdd, 0xff };
  ClutterColor col1 = { 0xff, 0xff, 0xff, 0xff };
  ClutterColor col2 = {    0,    0,    0, 0xff };

  if (gtk_clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
    g_error ("Unable to initialize GtkClutter");

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  notebook = gtk_notebook_new ();
  gtk_container_add (GTK_CONTAINER (window), notebook);

  clutter0 = gtk_clutter_embed_new ();
  gtk_widget_set_size_request (clutter0, 320, 320);
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), clutter0,
                            gtk_label_new ("One stage"));
  stage0 = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter0));
  clutter_stage_set_color (CLUTTER_STAGE (stage0), &col0);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox,
                            gtk_label_new ("Two stages"));

  clutter1 = gtk_clutter_embed_new ();
  gtk_widget_set_size_request (clutter1, 320, 240);
  stage1 = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter1));
  clutter_stage_set_color (CLUTTER_STAGE(stage1), &col1);
  tex1 = gtk_clutter_texture_new_from_stock (clutter1,
                                             GTK_STOCK_DIALOG_INFO,
                                             GTK_ICON_SIZE_DIALOG);
  clutter_actor_set_anchor_point (tex1,
                                  clutter_actor_get_width (tex1) / 2,
                                  clutter_actor_get_height (tex1) / 2);
  clutter_actor_set_position (tex1, 160, 120);
  clutter_stage_add (stage1, tex1); 
  clutter_actor_show (tex1);

  gtk_container_add (GTK_CONTAINER (vbox), clutter1);

  clutter2 = gtk_clutter_embed_new ();
  gtk_widget_set_size_request (clutter2, 320, 120);
  stage2 = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter2));
  clutter_stage_set_color (CLUTTER_STAGE(stage2), &col2);
  tex2 = gtk_clutter_texture_new_from_icon_name (clutter1,
                                                 "user-info",
                                                 GTK_ICON_SIZE_BUTTON);
  clutter_actor_set_anchor_point (tex2,
                                  clutter_actor_get_width (tex2) / 2,
                                  clutter_actor_get_height (tex2) / 2);
  clutter_actor_set_position (tex2, 160, 60);
  clutter_stage_add (stage2, tex2);

  gtk_container_add (GTK_CONTAINER (vbox), clutter2);

  g_signal_connect (stage2, "allocation-changed",
                    G_CALLBACK (on_stage2_allocation_changed),
                    tex2);

  gtk_widget_show_all (window);

  gtk_main();

  return 0;
}
