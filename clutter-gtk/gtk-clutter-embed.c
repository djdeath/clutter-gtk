/* gtk-clutter-embed.c: Embeddable ClutterStage
 *
 * Copyright (C) 2007 OpenedHand
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not see <http://www.fsf.org/licensing>.
 *
 * Authors:
 *   Iain Holmes  <iain@openedhand.com>
 *   Emmanuele Bassi  <ebassi@openedhand.com>
 */

/**
 * SECTION:gtk-clutter-embed
 * @short_description: Widget for embedding a Clutter scene
 *
 * #GtkClutterEmbed is a GTK+ widget embedding a #ClutterStage. Using
 * a #GtkClutterEmbed widget is possible to build, show and interact with
 * a scene built using Clutter inside a GTK+ application.
 *
 * <note>To avoid flickering on show, you should call gtk_widget_show()
 * or gtk_widget_realize() before calling clutter_actor_show() on the
 * embedded #ClutterStage actor. This is needed for Clutter to be able
 * to paint on the #GtkClutterEmbed widget.</note>
 *
 * Since: 0.6
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib-object.h>

#include <clutter/clutter-main.h>
#include <clutter/clutter-stage.h>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <clutter/clutter-x11.h>

#include "gtk-clutter-embed.h"

G_DEFINE_TYPE (GtkClutterEmbed, gtk_clutter_embed, GTK_TYPE_WIDGET);

#define GTK_CLUTTER_EMBED_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_CLUTTER_EMBED, GtkClutterEmbedPrivate))

struct _GtkClutterEmbedPrivate
{
  ClutterActor *stage;
};

static void
gtk_clutter_embed_send_configure (GtkClutterEmbed *embed)
{
  GtkWidget *widget;
  GdkEvent *event = gdk_event_new (GDK_CONFIGURE);

  widget = GTK_WIDGET (embed);

  event->configure.window = g_object_ref (widget->window);
  event->configure.send_event = TRUE;
  event->configure.x = widget->allocation.x;
  event->configure.y = widget->allocation.y;
  event->configure.width = widget->allocation.width;
  event->configure.height = widget->allocation.height;
  
  gtk_widget_event (widget, event);
  gdk_event_free (event);
}

static void
gtk_clutter_embed_dispose (GObject *gobject)
{
  G_OBJECT_CLASS (gtk_clutter_embed_parent_class)->dispose (gobject);
}

static void
gtk_clutter_embed_show (GtkWidget *widget)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  /* Make sure the widget is realised before we show */
  gtk_widget_realize (widget);

  GTK_WIDGET_CLASS (gtk_clutter_embed_parent_class)->show (widget);

  clutter_actor_show (priv->stage);
}

static void
gtk_clutter_embed_hide (GtkWidget *widget)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  GTK_WIDGET_CLASS (gtk_clutter_embed_parent_class)->hide (widget);

  clutter_actor_hide (priv->stage);
}

static void
gtk_clutter_embed_realize (GtkWidget *widget)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv; 
  GdkWindowAttr attributes;
  int attributes_mask;
  
  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
                                   &attributes,
                                   attributes_mask);
  gdk_window_set_user_data (widget->window, widget);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
  
  gdk_window_set_back_pixmap (widget->window, NULL, FALSE);

  clutter_x11_set_stage_foreign (CLUTTER_STAGE (priv->stage), 
                                 GDK_WINDOW_XID (widget->window));
  clutter_redraw (CLUTTER_STAGE(priv->stage));

  gtk_clutter_embed_send_configure (GTK_CLUTTER_EMBED (widget));
}

static void
gtk_clutter_embed_size_allocate (GtkWidget     *widget,
                                 GtkAllocation *allocation)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  widget->allocation = *allocation;

  if (GTK_WIDGET_REALIZED (widget))
    {
      gdk_window_move_resize (widget->window,
                              allocation->x, allocation->y,
                              allocation->width, allocation->height);

      gtk_clutter_embed_send_configure (GTK_CLUTTER_EMBED (widget));
    }

  clutter_actor_set_size (priv->stage,
                          allocation->width,
                          allocation->height);

  if (CLUTTER_ACTOR_IS_VISIBLE (priv->stage))
    clutter_actor_queue_redraw (priv->stage);
}

static gboolean
gtk_clutter_embed_button_event (GtkWidget      *widget,
                                GdkEventButton *event)
{
  ClutterEvent cevent = { 0, };

  if (event->type == GDK_BUTTON_PRESS ||
      event->type == GDK_2BUTTON_PRESS ||
      event->type == GDK_3BUTTON_PRESS)
    cevent.type = cevent.button.type = CLUTTER_BUTTON_PRESS;
  else if (event->type == GDK_BUTTON_RELEASE)
    cevent.type = cevent.button.type = CLUTTER_BUTTON_RELEASE;
  else
    return FALSE;

  cevent.button.x = event->x;
  cevent.button.y = event->y;
  cevent.button.time = event->time;
  cevent.button.click_count =
    (event->type == GDK_BUTTON_PRESS ? 1
                                     : (event->type == GDK_2BUTTON_PRESS ? 2
                                                                         : 3));
  cevent.button.modifier_state = event->state;
  cevent.button.button = event->button;

  clutter_do_event (&cevent);

  return TRUE;
}

static gboolean
gtk_clutter_embed_key_event (GtkWidget   *widget,
                             GdkEventKey *event)
{
  ClutterEvent cevent = { 0, };

  if (event->type == GDK_KEY_PRESS)
    cevent.type = cevent.key.type = CLUTTER_KEY_PRESS;
  else if (event->type == GDK_KEY_RELEASE)
    cevent.type = cevent.key.type = CLUTTER_KEY_RELEASE;
  else
    return FALSE;

  cevent.key.time = event->time;
  cevent.key.modifier_state = event->state;
  cevent.key.keyval = event->keyval;
  cevent.key.hardware_keycode = event->hardware_keycode;

  clutter_do_event (&cevent);

  return TRUE;
}

static gboolean
gtk_clutter_embed_expose_event (GtkWidget *widget, GdkEventExpose *event)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  if (CLUTTER_ACTOR_IS_VISIBLE (priv->stage))
    clutter_actor_queue_redraw (priv->stage);

  return TRUE;
}

static gboolean
gtk_clutter_embed_map_event (GtkWidget	     *widget,
                             GdkEventAny     *event)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  /* The backend wont get the XEvent as we go strait to do_event().
   * So we have to make sure we set the event here.
  */
  CLUTTER_ACTOR_SET_FLAGS (priv->stage, CLUTTER_ACTOR_MAPPED);

  return TRUE;
}

static void
gtk_clutter_embed_class_init (GtkClutterEmbedClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtkClutterEmbedPrivate));

  gobject_class->dispose = gtk_clutter_embed_dispose;

  widget_class->size_allocate = gtk_clutter_embed_size_allocate;
  widget_class->realize = gtk_clutter_embed_realize;
  widget_class->show = gtk_clutter_embed_show;
  widget_class->hide = gtk_clutter_embed_hide;
  widget_class->button_press_event = gtk_clutter_embed_button_event;
  widget_class->button_release_event = gtk_clutter_embed_button_event;
  widget_class->key_press_event = gtk_clutter_embed_key_event;
  widget_class->key_release_event = gtk_clutter_embed_key_event;
  widget_class->expose_event = gtk_clutter_embed_expose_event;
  widget_class->map_event = gtk_clutter_embed_map_event;
}

static void
gtk_clutter_embed_init (GtkClutterEmbed *embed)
{
  GtkClutterEmbedPrivate *priv;
  const XVisualInfo *xvinfo;
  GdkVisual *visual;
  GdkColormap *colormap;

  embed->priv = priv = GTK_CLUTTER_EMBED_GET_PRIVATE (embed);

  gtk_widget_set_double_buffered (GTK_WIDGET (embed), FALSE);

  /* note we never ref or unref this */
  priv->stage = clutter_stage_create_new (); 
  /* we always create new stages rather than use the default */
  /* priv->stage = clutter_stage_get_default (); */

  /* We need to use the colormap from the Clutter visual */
  xvinfo = clutter_x11_get_stage_visual (CLUTTER_STAGE (priv->stage));
  visual = gdk_x11_screen_lookup_visual (gdk_screen_get_default (),
                                         xvinfo->visualid);
  colormap = gdk_colormap_new (visual, FALSE);
  gtk_widget_set_colormap (GTK_WIDGET (embed), colormap);
}

/**
 * gtk_clutter_init:
 *
 * This function should be called instead of #clutter_init() and after
 * #gtk_init()
 *
 * Return value: %CLUTTER_INIT_SUCCESS on success, a negative integer
 *   on failure.
 *
 * Since: 0.8
 */
ClutterInitError
gtk_clutter_init (int *argc, char ***argv)
{
  clutter_x11_set_display (GDK_DISPLAY());
  clutter_x11_disable_event_retrieval ();

  /* FIXME: call gtk_init() here? */

  return clutter_init (argc, argv);
}

/**
 * gtk_clutter_embed_new:
 *
 * Creates a new #GtkClutterEmbed widget. This widget can be
 * used to build a scene using Clutter API into a GTK+ application.
 *
 * Return value: the newly created #GtkClutterEmbed
 *
 * Since: 0.6
 */
GtkWidget *
gtk_clutter_embed_new (void)
{
  return g_object_new (GTK_TYPE_CLUTTER_EMBED, NULL);
}

/**
 * gtk_clutter_embed_get_stage:
 * @embed: a #GtkClutterEmbed
 *
 * Retrieves the #ClutterStage from @embed. The returned stage can be
 * used to add actors to the Clutter scene.
 *
 * Return value: the Clutter stage. You should never destroy or unref
 *   the returned actor.
 *
 * Since: 0.6
 */
ClutterActor *
gtk_clutter_embed_get_stage (GtkClutterEmbed *embed)
{
  g_return_val_if_fail (GTK_IS_CLUTTER_EMBED (embed), NULL);

  return embed->priv->stage;
}
