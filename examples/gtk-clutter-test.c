#include "config.h"

#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <math.h>

#include <glib/gi18n.h>

#include <clutter-gtk/clutter-gtk.h>

#define NHANDS          2
#define WINWIDTH        400
#define WINHEIGHT       400
#define RADIUS          150

typedef struct SuperOH
{
  ClutterActor *stage;
  ClutterActor *hand[NHANDS], *bgtex;
  ClutterActor *group;
  GdkPixbuf    *bgpixb;

} SuperOH; 

static gboolean fade = FALSE;
static gboolean fullscreen = FALSE;

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
      if (a && (CLUTTER_IS_TEXTURE (a) || CLUTTER_IS_CLONE (a)))
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
  SuperOH        *oh = (SuperOH *)data;
  gint            i;
  guint           rotation = clutter_timeline_get_progress (timeline) * 360.0f;

  /* Rotate everything clockwise about stage center*/
  clutter_actor_set_rotation (CLUTTER_ACTOR (oh->group),
                              CLUTTER_Z_AXIS,
                              rotation,
                              WINWIDTH / 2, WINHEIGHT / 2, 0);

  for (i = 0; i < NHANDS; i++)
    {
      /* rotate each hand around there centers */
      clutter_actor_set_rotation (oh->hand[i],
                                  CLUTTER_Z_AXIS,
                                  - 6.0 * rotation,
                                  clutter_actor_get_width (oh->hand[i]) / 2,
                                  clutter_actor_get_height (oh->hand[i]) / 2,
                                  0);
      if (fade == TRUE)
        clutter_actor_set_opacity (oh->hand[i], (255 - (rotation % 255)));
    }
}

static void
clickity (GtkButton *button,
          gpointer ud)
{
        fade = !fade;
}

static void
on_fullscreen (GtkButton *button,
               GtkWindow *window)
{
  if (!fullscreen)
    {
      gtk_window_fullscreen (window);
      fullscreen = TRUE;
    }
  else
    {
      gtk_window_unfullscreen (window);
      fullscreen = FALSE;
    }
}

int
main (int argc, char *argv[])
{
  ClutterTimeline *timeline;
  ClutterActor    *stage;
  ClutterColor     stage_color = { 0x61, 0x64, 0x8c, 0xff };
  ClutterConstraint *constraint;
  GtkWidget       *window, *clutter;
  GtkWidget       *label, *button, *vbox;
  GdkPixbuf       *pixbuf;
  SuperOH         *oh;
  gint             i;
  GError          *error;

  error = NULL;
  gtk_clutter_init_with_args (&argc, &argv,
                              NULL,
                              NULL,
                              NULL,
                              &error);
  if (error)
    g_error ("Unable to initialize: %s", error->message);

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

  button = gtk_button_new_with_label ("Fullscreen");
  gtk_button_set_image (GTK_BUTTON (button),
                        gtk_image_new_from_stock (GTK_STOCK_FULLSCREEN,
                                                  GTK_ICON_SIZE_BUTTON));
  g_signal_connect (button, "clicked",
                    G_CALLBACK (on_fullscreen),
                    window);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  button = gtk_button_new_from_stock (GTK_STOCK_QUIT);
  g_signal_connect_swapped (button, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window);
  gtk_box_pack_end (GTK_BOX (vbox), button, FALSE, FALSE, 0);
  
  /* and its background color */

  clutter_stage_set_color (CLUTTER_STAGE (stage),
		           &stage_color);

  oh = g_new (SuperOH, 1);
  oh->stage = stage;

  /* create a new group to hold multiple actors in a group */
  oh->group = clutter_group_new ();
  
  for (i = 0; i < NHANDS; i++)
    {
      gint x, y, w, h;

      /* Create a texture from pixbuf, then clone in to same resources */
      if (i == 0)
        {
          oh->hand[i] = gtk_clutter_texture_new ();
          gtk_clutter_texture_set_from_pixbuf (GTK_CLUTTER_TEXTURE (oh->hand[i]), pixbuf, NULL);
        }
      else
        oh->hand[i] = clutter_clone_new (oh->hand[0]);

      /* Place around a circle */
      w = clutter_actor_get_width (oh->hand[0]);
      h = clutter_actor_get_height (oh->hand[0]);

      x = WINWIDTH/2  + RADIUS * cos (i * M_PI / (NHANDS/2)) - w/2;
      y = WINHEIGHT/2 + RADIUS * sin (i * M_PI / (NHANDS/2)) - h/2;

      clutter_actor_set_position (oh->hand[i], x, y);

      /* Add to our group group */
      clutter_container_add_actor (CLUTTER_CONTAINER (oh->group),
                                   oh->hand[i]);
    }

  /* Add the group to the stage */
  clutter_container_add_actor (CLUTTER_CONTAINER (stage),
                               CLUTTER_ACTOR (oh->group));

  constraint = clutter_align_constraint_new (oh->stage, CLUTTER_ALIGN_X_AXIS, 0.5);
  clutter_actor_add_constraint (oh->group, constraint);
  constraint = clutter_align_constraint_new (oh->stage, CLUTTER_ALIGN_Y_AXIS, 0.5);
  clutter_actor_add_constraint (oh->group, constraint);

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
  timeline = clutter_timeline_new (6000);
  g_object_set(timeline, "loop", TRUE, NULL);   /* have it loop */

  /* fire a callback for frame change */
  g_signal_connect(timeline, "new-frame",  G_CALLBACK (frame_cb), oh);

  /* and start it */
  clutter_timeline_start (timeline);

  gtk_main();

  return 0;
}
