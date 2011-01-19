#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <math.h>

#include <clutter-gtk/clutter-gtk.h>

#define NWIDGETS   4
#define WINWIDTH   400
#define WINHEIGHT  400
#define RADIUS     80

static ClutterActor *group = NULL;
static ClutterActor *widgets[NWIDGETS];
static gboolean do_rotate = TRUE;

/* Timeline handler */
void
frame_cb (ClutterTimeline *timeline,
	  gint             msecs,
	  gpointer         data)
{
  double rotation = clutter_timeline_get_progress (timeline) * 360.0;
  gint i;

  if (!do_rotate)
    return;

  /* Rotate everything clockwise about stage center */
  clutter_actor_set_rotation (CLUTTER_ACTOR (group),
                              CLUTTER_Z_AXIS,
                              rotation,
                              WINWIDTH / 2,
                              WINHEIGHT / 2,
                              0);

  for (i = 0; i < NWIDGETS; i++)
    {
      /* rotate each widget around its center */
      gfloat w = clutter_actor_get_width (widgets[i]);
      gfloat h = clutter_actor_get_height (widgets[i]);

      clutter_actor_set_rotation (widgets[i], CLUTTER_Z_AXIS,
                                  - 2 * rotation,
                                  w / 2,
                                  h / 2,
                                  0);
      clutter_actor_set_opacity (widgets[i], 50 * sin (2 * M_PI * rotation / 360) + (255 - 50));
    }
}

static void
button_clicked (GtkWidget *button,
		GtkWidget *vbox)
{
  GtkWidget *label;
  g_print ("button clicked\n");
  label = gtk_label_new ("A new label");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
}

static ClutterActor *
create_gtk_actor (int i)
{
  GtkWidget       *button, *vbox, *entry, *bin;
  ClutterActor    *gtk_actor;

  gtk_actor = gtk_clutter_actor_new ();
  bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (gtk_actor));

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_container_add (GTK_CONTAINER (bin), vbox);

  button = gtk_button_new_with_label ("A Button");
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  g_signal_connect (button, "clicked", G_CALLBACK (button_clicked), vbox);

  button = gtk_check_button_new_with_label ("Another button");
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  entry = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX (vbox), entry, FALSE, FALSE, 0);

  gtk_widget_show_all (bin);

  return gtk_actor;
}

int
main (int argc, char *argv[])
{
  ClutterTimeline *timeline;
  ClutterActor    *stage;
  ClutterColor     stage_color = { 0x61, 0x64, 0x8c, 0xff };
  GtkWidget       *window, *clutter;
  GtkWidget       *button, *vbox;
  gint             i;

  if (gtk_clutter_init_with_args (&argc, &argv, NULL, NULL, NULL, NULL) != CLUTTER_INIT_SUCCESS)
    g_error ("Unable to initialize GtkClutter");

  if (argc != 1)
    do_rotate = FALSE;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  clutter = gtk_clutter_embed_new ();
  gtk_widget_set_size_request (clutter, WINWIDTH, WINHEIGHT);

  gtk_box_pack_start (GTK_BOX (vbox), clutter, TRUE, TRUE, 0);

  stage = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter));

  button = gtk_button_new_from_stock (GTK_STOCK_QUIT);
  g_signal_connect_swapped (button, "clicked",
			    G_CALLBACK (gtk_widget_destroy),
			    window);
  gtk_box_pack_end (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  clutter_stage_set_color (CLUTTER_STAGE (stage), &stage_color);

  /* create a new group to hold multiple actors in a group */
  group = clutter_group_new ();

  for (i = 0; i < NWIDGETS; i++)
    {
      gint x, y, w, h;

      widgets[i] = create_gtk_actor (i);

      /* Add to our group group */
      clutter_container_add_actor (CLUTTER_CONTAINER (group), widgets[i]);

      /* Place around a circle */
      w = clutter_actor_get_width (widgets[0]);
      h = clutter_actor_get_height (widgets[0]);

      x = WINWIDTH/2  + RADIUS * cos (i * 2 * M_PI / (NWIDGETS)) - w/2;
      y = WINHEIGHT/2 + RADIUS * sin (i * 2 * M_PI / (NWIDGETS)) - h/2;

      clutter_actor_set_position (widgets[i], x, y);
    }

  /* Add the group to the stage and center it*/
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), group);
  clutter_actor_add_constraint (group, clutter_align_constraint_new (stage, CLUTTER_ALIGN_X_AXIS, 0.5));
  clutter_actor_add_constraint (group, clutter_align_constraint_new (stage, CLUTTER_ALIGN_Y_AXIS, 0.5));

  gtk_widget_show_all (window);

  /* Only show the actors after parent show otherwise it will just be
   * unrealized when the clutter foreign window is set. widget_show
   * will call show on the stage.
   */
  clutter_actor_show_all (CLUTTER_ACTOR (group));

  /* Create a timeline to manage animation */
  timeline = clutter_timeline_new (6000);
  clutter_timeline_set_loop (timeline, TRUE);

  /* fire a callback for frame change */
  g_signal_connect (timeline, "new-frame",  G_CALLBACK (frame_cb), stage);

  /* and start it */
  clutter_timeline_start (timeline);

  gtk_main();

  return 0;
}
