#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <math.h>

#include <clutter-gtk/clutter-gtk.h>

#define NWIDGETS   5
#define WINWIDTH   400
#define WINHEIGHT  400
#define RADIUS     80

static ClutterGroup *group = NULL;
static ClutterActor *widgets[NWIDGETS];

static gboolean do_rotate = TRUE;

/* input handler */
void
input_cb (ClutterStage *stage,
	  ClutterEvent *event,
	  gpointer      data)
{
  if (event->type == CLUTTER_BUTTON_PRESS)
    {
      ClutterActor *a;
      gfloat x, y;

      clutter_event_get_coords (event, &x, &y);

      a = clutter_stage_get_actor_at_pos (stage, CLUTTER_PICK_ALL, x, y);
      g_print ("click at %f, %f -> %s:%p\n",
	       (double)x, (double)y,
	       g_type_name_from_instance ((GTypeInstance *)a), a);
      if (a && !CLUTTER_IS_STAGE (a) && ! CLUTTER_IS_GROUP (a))
	clutter_actor_hide (a);
    }
  else if (event->type == CLUTTER_KEY_PRESS)
    {
      g_print ("*** key press event (key:%c) ***\n",
	       clutter_event_get_key_symbol (event));

      if (clutter_event_get_key_symbol (event) == CLUTTER_q)
	gtk_main_quit ();
    }
}


/* Timeline handler */
void
frame_cb (ClutterTimeline *timeline,
	  gint             msecs,
	  gpointer         data)
{
  gint i;
  double rotation = clutter_timeline_get_progress (timeline) * 360.0;

  if (!do_rotate)
    return;

  /* Rotate everything clockwise about stage center */
  clutter_actor_set_rotation (CLUTTER_ACTOR (group),
			      CLUTTER_Z_AXIS,
			      rotation,
			      WINWIDTH / 2, WINHEIGHT / 2, 0);

  for (i = 0; i < NWIDGETS; i++)
    {
      /* rotate each widget around its center */
      gint w = clutter_actor_get_width (widgets[i]);
      gint h = clutter_actor_get_height (widgets[i]);
      clutter_actor_set_rotation (widgets[i],
				  CLUTTER_Z_AXIS,
				  - 2 * rotation,
				  w / 2, h / 2,
				  0);
      clutter_actor_set_opacity (widgets[i], 50 * sin (2 * M_PI * rotation / 360) + (255 - 50));
    }

  /*
  clutter_actor_rotate_x (CLUTTER_ACTOR(oh->group),
			    75.0,
			    WINHEIGHT/2, 0);
  */
}

#if 0
static ClutterActor *
create_gtk_actor (int i)
{
  ClutterColor colour = { 255, 0, 0, 255 };
  ClutterActor *actor;

  actor = clutter_rectangle_new_with_color (&colour);
  clutter_actor_set_size (actor, 123, 60);
  return actor;
}
#else

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
  GtkWidget       *button, *vbox, *entry, *bin, *scroll;
  ClutterActor    *gtk_actor;
  int j;

  gtk_actor = gtk_clutter_actor_new ();
  bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (gtk_actor));

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_container_add (GTK_CONTAINER (bin), scroll);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), vbox);

  button = gtk_button_new_with_label ("A Button");
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  g_signal_connect (button, "clicked",
		    G_CALLBACK (button_clicked),
		    vbox);

  for (j = 0; j < 6; j++)
    {
      button = gtk_check_button_new_with_label ("Another button");
      gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
    }

  entry = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX (vbox), entry, FALSE, FALSE, 0);

  gtk_widget_show_all (bin);

  return gtk_actor;
}
#endif

int
main (int argc, char *argv[])
{
  ClutterTimeline *timeline;
  ClutterActor    *stage;
  ClutterColor     stage_color = { 0x61, 0x64, 0x8c, 0xff };
  GtkWidget       *window, *clutter;
  GtkWidget       *button, *vbox;
  gint             i;

  if (gtk_clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
    g_error ("Unable to initialize GtkClutter");

  if (argc != 1)
    do_rotate = FALSE;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy",
		    G_CALLBACK (gtk_main_quit), NULL);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  clutter = gtk_clutter_embed_new ();
  gtk_widget_set_size_request (clutter, WINWIDTH, WINHEIGHT);

  gtk_container_add (GTK_CONTAINER (vbox), clutter);

  stage = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter));

  button = gtk_button_new_from_stock (GTK_STOCK_QUIT);
  g_signal_connect_swapped (button, "clicked",
			    G_CALLBACK (gtk_widget_destroy),
			    window);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  /* and its background color */

  clutter_stage_set_color (CLUTTER_STAGE (stage),
			   &stage_color);

  /* create a new group to hold multiple actors in a group */
  group = CLUTTER_GROUP (clutter_group_new ());

  for (i = 0; i < NWIDGETS; i++)
    {
      gint x, y, w, h;

      widgets[i] = create_gtk_actor (i);

      /* Place around a circle */
      w = clutter_actor_get_width (widgets[0]);
      h = clutter_actor_get_height (widgets[0]);

      x = WINWIDTH/2  + RADIUS * cos (i * 2 * M_PI / (NWIDGETS)) - w/2;
      y = WINHEIGHT/2 + RADIUS * sin (i * 2 * M_PI / (NWIDGETS)) - h/2;

      clutter_actor_set_position (widgets[i], x, y);

      /* Add to our group group */
#if 1
      clutter_group_add (group, widgets[i]);
#else
      clutter_container_add_actor (CLUTTER_CONTAINER (stage),
				   CLUTTER_ACTOR (widgets[i]));
#endif
    }

  /* Add the group to the stage */
  clutter_container_add_actor (CLUTTER_CONTAINER (stage),
			       CLUTTER_ACTOR (group));

  g_signal_connect (stage, "button-press-event",
		    G_CALLBACK (input_cb),
		    NULL);
  g_signal_connect (stage, "key-release-event",
		    G_CALLBACK (input_cb),
		    NULL);

  gtk_widget_show_all (window);

  /* Only show the actors after parent show otherwise it will just be
   * unrealized when the clutter foreign window is set. widget_show
   * will call show on the stage.
   */
  clutter_actor_show_all (CLUTTER_ACTOR (group));

  /* Create a timeline to manage animation */
  timeline = clutter_timeline_new (6000);
  g_object_set(timeline, "loop", TRUE, NULL);   /* have it loop */

  /* fire a callback for frame change */
  g_signal_connect(timeline, "new-frame",  G_CALLBACK (frame_cb), NULL);

  /* and start it */
  clutter_timeline_start (timeline);

  gtk_main();

  return 0;
}
