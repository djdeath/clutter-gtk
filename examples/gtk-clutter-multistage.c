#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <clutter-gtk/gtk-clutter-embed.h>

int
main (int argc, char *argv[])
{
  ClutterTimeline *timeline;
  ClutterActor    *stage1, *stage2, *tex1, *tex2;
  ClutterColor     stage_color = { 0x61, 0x64, 0x8c, 0xff };
  GtkWidget       *window, *clutter1, *clutter2;
  GtkWidget       *label, *button, *vbox;
  GdkPixbuf       *pixbuf;
  gint             i;
  ClutterColor     col1 = { 0xff, 0xff, 0xff, 0xff };
  ClutterColor     col2 = { 0, 0, 0, 0xff };

  if (gtk_clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
    g_error ("Unable to initialize GtkClutter");

  pixbuf = gdk_pixbuf_new_from_file ("redhand.png", NULL);

  if (!pixbuf)
    g_error("pixbuf load failed");

  tex1 = clutter_texture_new_from_pixbuf (pixbuf);
  tex2 = clutter_clone_texture_new (tex1);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  clutter1 = gtk_clutter_embed_new ();
  gtk_widget_set_size_request (clutter1, 320, 240);
  stage1 = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter1));
  clutter_stage_set_color (CLUTTER_STAGE(stage1), &col1);
  clutter_stage_add (stage1, tex1); 

  gtk_container_add (GTK_CONTAINER (vbox), clutter1);

  clutter2 = gtk_clutter_embed_new ();
  gtk_widget_set_size_request (clutter2, 320, 240);
  stage2 = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter2));

  clutter_stage_set_color (CLUTTER_STAGE(stage2), &col2);
  clutter_stage_add (stage2, tex2);

  gtk_container_add (GTK_CONTAINER (vbox), clutter2);

  gtk_widget_show_all (window);
  clutter_actor_show_all (stage1); 
  clutter_actor_show_all (stage2); 

  gtk_main();

  return 0;
}
