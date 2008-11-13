#include <gtk/gtk.h>
#include <clutter/clutter.h>

#include <clutter-gtk/clutter-gtk.h>

int
main (int argc, char *argv[])
{
  ClutterTimeline *timeline;
  ClutterActor    *stage, *viewport, *tex;
  ClutterColor     stage_color = { 0x61, 0x64, 0x8c, 0xff };
  GtkWidget       *window, *embed; 
  GtkWidget       *table, *scrollbar;
  GtkAdjustment   *h_adjustment, *v_adjustment;
  gint             i;
  ClutterColor     col2 = { 0, };

  if (gtk_clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
    g_error ("Unable to initialize GtkClutter");

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  table = gtk_table_new (2, 2, FALSE);
  gtk_container_add (GTK_CONTAINER (window), table);
  gtk_widget_show (table);

  embed = gtk_clutter_embed_new ();
  gtk_table_attach (GTK_TABLE (table), embed,
                    0, 1,
                    0, 1,
                    GTK_EXPAND | GTK_FILL,
                    GTK_EXPAND | GTK_FILL,
                    0, 0);
  gtk_widget_set_size_request (embed, 320, 240);
  gtk_widget_show (embed);

  stage = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (embed));
  clutter_stage_set_color (CLUTTER_STAGE (stage), &stage_color);

  viewport = gtk_clutter_viewport_new (NULL, NULL);
  clutter_actor_set_size (viewport, 320, 240);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), viewport);

  if (argc < 2)
    tex = gtk_clutter_texture_new_from_stock (embed,
                                              GTK_STOCK_DIALOG_INFO,
                                              GTK_ICON_SIZE_DIALOG);
  else
    {
      GError *error = NULL;

      tex = clutter_texture_new_from_file (argv[1], &error);
      if (error)
        {
          g_warning ("Unable to open `%s': %s", argv[1], error->message);
          g_error_free (error);
        }
    }

  clutter_container_add_actor (CLUTTER_CONTAINER (viewport), tex); 

  gtk_clutter_scrollable_get_adjustments (GTK_CLUTTER_SCROLLABLE (viewport),
                                          &h_adjustment,
                                          &v_adjustment);

  scrollbar = gtk_vscrollbar_new (v_adjustment);
  gtk_table_attach (GTK_TABLE (table), scrollbar,
                    1, 2,
                    0, 1,
                    0, GTK_EXPAND | GTK_FILL,
                    0, 0);
  gtk_widget_show (scrollbar);

  scrollbar = gtk_hscrollbar_new (h_adjustment);
  gtk_table_attach (GTK_TABLE (table), scrollbar,
                    0, 1,
                    1, 2,
                    GTK_EXPAND | GTK_FILL, 0,
                    0, 0);
  gtk_widget_show (scrollbar);

  gtk_widget_show (window);

  gtk_main();

  return 0;
}
