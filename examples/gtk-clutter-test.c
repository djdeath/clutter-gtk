#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <math.h>

#include <clutter-gtk/gtk-clutter-embed.h>
#include <clutter-gtk/gtk-clutter-util.h>

#define TRAILS 0
#define NHANDS  2
#define WINWIDTH   400
#define WINHEIGHT  400
#define RADIUS     150

typedef struct SuperOH
{
  ClutterActor *hand[NHANDS], *bgtex;
  ClutterGroup   *group;
  GdkPixbuf      *bgpixb;

} SuperOH; 

gboolean fade = FALSE;

/* input handler */
void 
input_cb (ClutterStage *stage,
	  ClutterEvent *event,
	  gpointer      data)
{
  if (event->type == CLUTTER_BUTTON_PRESS)
    {
      ClutterActor *a;
      gint x, y;

      clutter_event_get_coords (event, &x, &y);

      a = clutter_stage_get_actor_at_pos (stage, x, y);
      if (a && (CLUTTER_IS_TEXTURE (a) || CLUTTER_IS_CLONE_TEXTURE (a)))
	clutter_actor_hide (a);
    }
  else if (event->type == CLUTTER_KEY_PRESS)
    {
      ClutterKeyEvent *kev = (ClutterKeyEvent *) event;

      g_print ("*** key press event (key:%c) ***\n",
	       clutter_key_event_symbol (kev));
      
      if (clutter_key_event_symbol (kev) == CLUTTER_q)
	gtk_main_quit ();
    }
}


/* Timeline handler */
void
frame_cb (ClutterTimeline *timeline, 
	  gint             frame_num, 
	  gpointer         data)
{
  SuperOH        *oh = (SuperOH *)data;
  gint            i;

#if TRAILS
  oh->bgpixb = clutter_stage_snapshot (CLUTTER_STAGE (stage),
				       0, 0,
				       WINWIDTH,
				       WINHEIGHT);
  clutter_texture_set_pixbuf (CLUTTER_TEXTURE (oh->bgtex), oh->bgpixb);
  g_object_unref (G_OBJECT (oh->bgpixb));
  g_object_unref (stage);
#endif

  /* Rotate everything clockwise about stage center*/
  clutter_actor_set_rotation (CLUTTER_ACTOR (oh->group),
                              CLUTTER_Z_AXIS,
                              frame_num,
                              WINWIDTH / 2, WINHEIGHT / 2, 0);

  for (i = 0; i < NHANDS; i++)
    {
      /* rotate each hand around there centers */
      clutter_actor_set_rotation (oh->hand[i],
                                  CLUTTER_Z_AXIS,
                                  - 6.0 * frame_num,
                                  clutter_actor_get_width (oh->hand[i]) / 2,
                                  clutter_actor_get_height (oh->hand[i]) / 2,
                                  0);
      if (fade == TRUE)
        clutter_actor_set_opacity (oh->hand[i], (255 - (frame_num % 255)));
    }

  /*
  clutter_actor_rotate_x (CLUTTER_ACTOR(oh->group),
			    75.0,
			    WINHEIGHT/2, 0);
  */
}

static void
clickity (GtkButton *button,
          gpointer ud)
{
        fade = !fade;
}

int
main (int argc, char *argv[])
{
  ClutterTimeline *timeline;
  ClutterActor    *stage;
  ClutterColor     stage_color = { 0x61, 0x64, 0x8c, 0xff };
  GtkWidget       *window, *clutter, *socket_box;
  GtkWidget       *label, *button, *vbox;
  GdkPixbuf       *pixbuf;
  SuperOH         *oh;
  gint             i;

  if (gtk_clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
    g_error ("Unable to initialize GtkClutter");

  pixbuf = gdk_pixbuf_new_from_file ("redhand.png", NULL);

  if (!pixbuf)
    g_error("pixbuf load failed");

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  clutter = gtk_clutter_embed_new ();
  gtk_widget_set_size_request (clutter, WINWIDTH, WINHEIGHT);

  gtk_container_add (GTK_CONTAINER (vbox), clutter);

  stage = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter));

  label = gtk_label_new ("This is a label");
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

  button = gtk_button_new_with_label ("This is a button...clicky");
  g_signal_connect (button, "clicked",
                    G_CALLBACK (clickity), NULL);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  button = gtk_button_new_from_stock (GTK_STOCK_QUIT);
  g_signal_connect_swapped (button, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window);
  gtk_box_pack_end (GTK_BOX (vbox), button, FALSE, FALSE, 0);
  
  /* and its background color */

  clutter_stage_set_color (CLUTTER_STAGE (stage),
		           &stage_color);

  oh = g_new(SuperOH, 1);

#if TRAILS
  oh->bgtex = clutter_texture_new();
  clutter_actor_set_size (oh->bgtex, WINWIDTH, WINHEIGHT);
  clutter_actor_set_opacity (oh->bgtex, 0x99);
  clutter_group_add (CLUTTER_GROUP (stage), oh->bgtex);
#endif

  /* create a new group to hold multiple actors in a group */
  oh->group = CLUTTER_GROUP (clutter_group_new ());
  
  for (i = 0; i < NHANDS; i++)
    {
      gint x, y, w, h;
#if 1
      /* Create a texture from pixbuf, then clone in to same resources */
      if (i == 0)
       oh->hand[i] = gtk_clutter_texture_new_from_pixbuf (pixbuf);
     else
       oh->hand[i] = clutter_clone_texture_new (CLUTTER_TEXTURE (oh->hand[0]));
#else
      ClutterColor colour = { 255, 0, 0, 255 };

      oh->hand[i] = clutter_rectangle_new_with_color (&colour);
      clutter_actor_set_size (oh->hand[i], 50, 50);
#endif
      /* Place around a circle */
      w = clutter_actor_get_width (oh->hand[0]);
      h = clutter_actor_get_height (oh->hand[0]);

      x = WINWIDTH/2  + RADIUS * cos (i * M_PI / (NHANDS/2)) - w/2;
      y = WINHEIGHT/2 + RADIUS * sin (i * M_PI / (NHANDS/2)) - h/2;

      clutter_actor_set_position (oh->hand[i], x, y);

      /* Add to our group group */
      clutter_group_add (oh->group, oh->hand[i]);
    }

  /* Add the group to the stage */
  clutter_container_add_actor (CLUTTER_CONTAINER (stage),
                               CLUTTER_ACTOR (oh->group));

  g_signal_connect (stage, "button-press-event",
		    G_CALLBACK (input_cb), 
		    oh);
  g_signal_connect (stage, "key-release-event",
		    G_CALLBACK (input_cb),
		    oh);

  gtk_widget_show_all (window);

  /* Only show the actors after parent show otherwise it will just be
   * unrealized when the clutter foreign window is set. widget_show
   * will call show on the stage.
   */
  clutter_actor_show_all (CLUTTER_ACTOR (oh->group));

  /* Create a timeline to manage animation */
  timeline = clutter_timeline_new (360, 60); /* num frames, fps */
  g_object_set(timeline, "loop", TRUE, NULL);   /* have it loop */

  /* fire a callback for frame change */
  g_signal_connect(timeline, "new-frame",  G_CALLBACK (frame_cb), oh);

  /* and start it */
  clutter_timeline_start (timeline);

  gtk_main();

  return 0;
}
