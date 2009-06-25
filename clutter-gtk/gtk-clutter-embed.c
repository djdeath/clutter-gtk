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

#include "gtk-clutter-embed.h"

#include <glib-object.h>

#include <gdk/gdk.h>

#if defined(HAVE_CLUTTER_GTK_X11)

#include <clutter/x11/clutter-x11.h>
#include <gdk/gdkx.h>

#elif defined(HAVE_CLUTTER_GTK_WIN32)

#include <clutter/clutter-win32.h>
#include <gdk/gdkwin32.h>

#endif /* HAVE_CLUTTER_GTK_{X11,WIN32} */

G_DEFINE_TYPE (GtkClutterEmbed, gtk_clutter_embed, GTK_TYPE_CONTAINER);

#define GTK_CLUTTER_EMBED_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_CLUTTER_TYPE_EMBED, GtkClutterEmbedPrivate))

struct _GtkClutterEmbedPrivate
{
  ClutterActor *stage;

  guint queue_redraw_id;
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
on_stage_queue_redraw (ClutterStage *stage,
                       ClutterActor *origin,
                       gpointer      user_data)
{
  GtkWidget *embed = user_data;

  /* we stop the emission of the Stage::queue-redraw signal to prevent
   * the default handler from running; then we queue a redraw on the
   * GtkClutterEmbed widget which will cause an expose event to be
   * emitted. the Stage is redrawn in the expose event handler, thus
   * "slaving" the Clutter redraw cycle to GTK+'s own
   */
  g_signal_stop_emission_by_name (stage, "queue-redraw");

  gtk_widget_queue_draw (embed);
}

static void
gtk_clutter_embed_dispose (GObject *gobject)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (gobject)->priv;

  if (priv->queue_redraw_id)
    {
      g_signal_handler_disconnect (priv->stage, priv->queue_redraw_id);
      priv->queue_redraw_id = 0;
    }

  if (priv->stage)
    {
      clutter_actor_destroy (priv->stage);
      priv->stage = NULL;
    }

  G_OBJECT_CLASS (gtk_clutter_embed_parent_class)->dispose (gobject);
}

static void
gtk_clutter_embed_show (GtkWidget *widget)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  if (GTK_WIDGET_REALIZED (widget))
    clutter_actor_show (priv->stage);

  GTK_WIDGET_CLASS (gtk_clutter_embed_parent_class)->show (widget);
}

static void
gtk_clutter_embed_hide (GtkWidget *widget)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  /* gtk emits a hide signal during dispose, so it's possible we may
   * have already disposed priv->stage. */
  if (priv->stage)
    clutter_actor_hide (priv->stage);

  GTK_WIDGET_CLASS (gtk_clutter_embed_parent_class)->hide (widget);
}

static void
gtk_clutter_embed_realize (GtkWidget *widget)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv; 
  GdkWindowAttr attributes;
  int attributes_mask;

#ifdef HAVE_CLUTTER_GTK_X11
  {
    const XVisualInfo *xvinfo;
    GdkVisual *visual;
    GdkColormap *colormap;

    /* We need to use the colormap from the Clutter visual */
    xvinfo = clutter_x11_get_stage_visual (CLUTTER_STAGE (priv->stage));
    if (xvinfo == None)
      {
        g_critical ("Unable to retrieve the XVisualInfo from Clutter");
        return;
      }

    visual = gdk_x11_screen_lookup_visual (gtk_widget_get_screen (widget),
                                           xvinfo->visualid);
    colormap = gdk_colormap_new (visual, FALSE);
    gtk_widget_set_colormap (widget, colormap);
  }
#endif

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);

  /* NOTE: GDK_MOTION_NOTIFY above should be safe as Clutter does its own
   *       throtling. 
  */
  attributes.event_mask = gtk_widget_get_events (widget)
                        | GDK_EXPOSURE_MASK
                        | GDK_BUTTON_PRESS_MASK
                        | GDK_BUTTON_RELEASE_MASK
                        | GDK_KEY_PRESS_MASK
                        | GDK_KEY_RELEASE_MASK
                        | GDK_POINTER_MOTION_MASK;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
                                   &attributes,
                                   attributes_mask);
  gdk_window_set_user_data (widget->window, widget);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
  
  gdk_window_set_back_pixmap (widget->window, NULL, FALSE);

#if defined(HAVE_CLUTTER_GTK_X11)
  clutter_x11_set_stage_foreign (CLUTTER_STAGE (priv->stage), 
                                 GDK_WINDOW_XID (widget->window));
#elif defined(HAVE_CLUTTER_GTK_WIN32)
  clutter_win32_set_stage_foreign (CLUTTER_STAGE (priv->stage), 
				   GDK_WINDOW_HWND (widget->window));
#endif /* HAVE_CLUTTER_GTK_{X11,WIN32} */

  clutter_actor_realize (priv->stage);

  if (GTK_WIDGET_VISIBLE (widget))
    clutter_actor_show (priv->stage);

  gtk_clutter_embed_send_configure (GTK_CLUTTER_EMBED (widget));
}

static void
gtk_clutter_embed_unrealize (GtkWidget *widget)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  clutter_actor_hide (priv->stage);

  GTK_WIDGET_CLASS (gtk_clutter_embed_parent_class)->unrealize (widget);
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

  /* change the size of the stage and ensure that the viewport
   * has been updated as well
   */
  clutter_actor_set_size (priv->stage,
                          allocation->width,
                          allocation->height);

  clutter_stage_ensure_viewport (CLUTTER_STAGE (priv->stage));
}

static gboolean
gtk_clutter_embed_expose_event (GtkWidget *widget,
                                GdkEventExpose *event)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  /* force a redraw on expose */
  clutter_redraw (CLUTTER_STAGE (priv->stage));

  return FALSE;
}

static gboolean
gtk_clutter_embed_motion_notify_event (GtkWidget      *widget,
                                       GdkEventMotion *event)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;
  ClutterEvent cevent = { 0, };

  cevent.type = CLUTTER_MOTION;
  cevent.any.stage = CLUTTER_STAGE (priv->stage);
  cevent.motion.x = event->x;
  cevent.motion.y = event->y;
  cevent.motion.time = event->time;
  cevent.motion.modifier_state = event->state;

  clutter_do_event (&cevent);

  /* doh - motion events can push ENTER/LEAVE events onto Clutters
   * internal event queue which we do really ever touch (essentially
   * proxying from gtks queue). The below pumps them back out and
   * processes.
   * *could* be side effects with below though doubful as no other
   * events reach the queue (we shut down event collection). Maybe
   * a peek_mask type call could be even safer. 
  */
  while (clutter_events_pending())
    {
      ClutterEvent *ev = clutter_event_get ();
      if (ev)
        {
          clutter_do_event (ev);
          clutter_event_free (ev);
        }
    }

  return FALSE;
}

static gboolean
gtk_clutter_embed_button_event (GtkWidget      *widget,
                                GdkEventButton *event)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;
  ClutterEvent cevent = { 0, };

  if (event->type == GDK_BUTTON_PRESS ||
      event->type == GDK_2BUTTON_PRESS ||
      event->type == GDK_3BUTTON_PRESS)
    cevent.type = cevent.button.type = CLUTTER_BUTTON_PRESS;
  else if (event->type == GDK_BUTTON_RELEASE)
    cevent.type = cevent.button.type = CLUTTER_BUTTON_RELEASE;
  else
    return FALSE;

  cevent.any.stage = CLUTTER_STAGE (priv->stage);
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

  return FALSE;
}

static gboolean
gtk_clutter_embed_key_event (GtkWidget   *widget,
                             GdkEventKey *event)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;
  ClutterEvent cevent = { 0, };

  if (event->type == GDK_KEY_PRESS)
    cevent.type = cevent.key.type = CLUTTER_KEY_PRESS;
  else if (event->type == GDK_KEY_RELEASE)
    cevent.type = cevent.key.type = CLUTTER_KEY_RELEASE;
  else
    return FALSE;

  cevent.any.stage = CLUTTER_STAGE (priv->stage);
  cevent.key.time = event->time;
  cevent.key.modifier_state = event->state;
  cevent.key.keyval = event->keyval;
  cevent.key.hardware_keycode = event->hardware_keycode;
  cevent.key.unicode_value = gdk_keyval_to_unicode (event->keyval);

  clutter_do_event (&cevent);

  return FALSE;
}

static gboolean
gtk_clutter_embed_map_event (GtkWidget	 *widget,
                             GdkEventAny *event)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;
  GtkWidgetClass *parent_class;
  gboolean res = FALSE;

  parent_class = GTK_WIDGET_CLASS (gtk_clutter_embed_parent_class);
  if (parent_class->map_event)
    res = parent_class->map_event (widget, event);

  clutter_actor_map (priv->stage);

  return res;
}

static gboolean
gtk_clutter_embed_unmap_event (GtkWidget   *widget,
                               GdkEventAny *event)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;
  GtkWidgetClass *parent_class;
  gboolean res = FALSE;

  parent_class = GTK_WIDGET_CLASS (gtk_clutter_embed_parent_class);
  if (parent_class->unmap_event)
    res = parent_class->unmap_event (widget, event);

  clutter_actor_unmap (priv->stage);

  return res;
}

static void
gtk_clutter_embed_unmap (GtkWidget *widget)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  clutter_actor_unmap (priv->stage);

  GTK_WIDGET_CLASS (gtk_clutter_embed_parent_class)->unmap (widget);
}

static gboolean
gtk_clutter_embed_focus_in (GtkWidget     *widget,
                            GdkEventFocus *event)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  g_signal_emit_by_name (priv->stage, "activate");

  return FALSE;
}

static gboolean
gtk_clutter_embed_focus_out (GtkWidget     *widget,
                             GdkEventFocus *event)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  g_signal_emit_by_name (priv->stage, "deactivate");

  /* give back key focus to the stage */
  clutter_stage_set_key_focus (CLUTTER_STAGE (priv->stage), NULL);

  return FALSE;
}

static gboolean
gtk_clutter_embed_scroll_event (GtkWidget      *widget,
                                GdkEventScroll *event)
{
  GtkClutterEmbedPrivate *priv = GTK_CLUTTER_EMBED (widget)->priv;

  ClutterEvent cevent = { 0, };

  if (event->type == GDK_SCROLL)
    cevent.type = cevent.scroll.type = CLUTTER_SCROLL;
  else
    return FALSE;

  cevent.any.stage = CLUTTER_STAGE (priv->stage);
  cevent.scroll.x = (gint) event->x;
  cevent.scroll.y = (gint) event->y;
  cevent.scroll.time = event->time;
  cevent.scroll.direction = event->direction;
  cevent.scroll.modifier_state = event->state;

  clutter_do_event (&cevent);

  return FALSE;
}

static void
gtk_clutter_embed_style_set (GtkWidget *widget,
                             GtkStyle  *old_style)
{
  GdkScreen *screen;
  GtkSettings *settings;
  gdouble dpi;
  gchar *font_name;
  const cairo_font_options_t *font_options;
  gint double_click_time, double_click_distance;
  ClutterBackend *backend;

  GTK_WIDGET_CLASS (gtk_clutter_embed_parent_class)->style_set (widget,
                                                                old_style);

  if (gtk_widget_has_screen (widget))
    screen = gtk_widget_get_screen (widget);
  else
    screen = gdk_screen_get_default ();

  dpi = gdk_screen_get_resolution (screen);
  if (dpi < 0)
    dpi = 96.0;

  font_options = gdk_screen_get_font_options (screen);

  settings = gtk_settings_get_for_screen (screen);
  g_object_get (G_OBJECT (settings),
                "gtk-font-name", &font_name,
                "gtk-double-click-time", &double_click_time,
                "gtk-double-click-distance", &double_click_distance,
                NULL);

  /* copy all settings and values coming from GTK+ into
   * the ClutterBackend; this way, a scene embedded into
   * a GtkClutterEmbed will not look completely alien
   */
  backend = clutter_get_default_backend ();
  clutter_backend_set_resolution (backend, dpi);
  clutter_backend_set_font_options (backend, font_options);
  clutter_backend_set_font_name (backend, font_name);
  clutter_backend_set_double_click_time (backend, double_click_time);
  clutter_backend_set_double_click_distance (backend, double_click_distance);

  g_free (font_name);
}

static void
gtk_clutter_embed_add (GtkContainer	 *container,
		       GtkWidget	 *widget)
{
  g_warning ("GtkClutterEmbed children not yet supported");
}

static void
gtk_clutter_embed_remove (GtkContainer	 *container,
			  GtkWidget	 *widget)
{
  g_warning ("GtkClutterEmbed children not yet supported");
}

static void
gtk_clutter_embed_forall (GtkContainer	 *container,
			  gboolean	  include_internals,
			  GtkCallback	  callback,
			  gpointer	  callback_data)
{
}

static GType
gtk_clutter_embed_child_type (GtkContainer *container)
{
  /* No children supported yet */
  return G_TYPE_NONE;
}

static void
gtk_clutter_embed_class_init (GtkClutterEmbedClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtkClutterEmbedPrivate));

  gobject_class->dispose = gtk_clutter_embed_dispose;

  widget_class->style_set = gtk_clutter_embed_style_set;
  widget_class->size_allocate = gtk_clutter_embed_size_allocate;
  widget_class->realize = gtk_clutter_embed_realize;
  widget_class->unrealize = gtk_clutter_embed_unrealize;
  widget_class->show = gtk_clutter_embed_show;
  widget_class->hide = gtk_clutter_embed_hide;
  widget_class->unmap = gtk_clutter_embed_unmap;
  widget_class->expose_event = gtk_clutter_embed_expose_event;
  widget_class->button_press_event = gtk_clutter_embed_button_event;
  widget_class->button_release_event = gtk_clutter_embed_button_event;
  widget_class->key_press_event = gtk_clutter_embed_key_event;
  widget_class->key_release_event = gtk_clutter_embed_key_event;
  widget_class->motion_notify_event = gtk_clutter_embed_motion_notify_event;
  widget_class->expose_event = gtk_clutter_embed_expose_event;
  widget_class->map_event = gtk_clutter_embed_map_event;
  widget_class->unmap_event = gtk_clutter_embed_unmap_event;
  widget_class->focus_in_event = gtk_clutter_embed_focus_in;
  widget_class->focus_out_event = gtk_clutter_embed_focus_out;
  widget_class->scroll_event = gtk_clutter_embed_scroll_event;

  container_class->add = gtk_clutter_embed_add;
  container_class->remove = gtk_clutter_embed_remove;
  container_class->forall = gtk_clutter_embed_forall;
  container_class->child_type = gtk_clutter_embed_child_type;
}

static void
gtk_clutter_embed_init (GtkClutterEmbed *embed)
{
  GtkClutterEmbedPrivate *priv;

  embed->priv = priv = GTK_CLUTTER_EMBED_GET_PRIVATE (embed);

  GTK_WIDGET_SET_FLAGS (embed, GTK_CAN_FOCUS);
  GTK_WIDGET_UNSET_FLAGS (embed, GTK_NO_WINDOW);

  /* disable double-buffering: it's automatically provided
   * by OpenGL
   */
  gtk_widget_set_double_buffered (GTK_WIDGET (embed), FALSE);

  /* we always create new stages rather than use the default */
  priv->stage = clutter_stage_new ();

  /* intercept the queue-redraw signal of the stage to know when
   * Clutter-side requests a redraw; this way we can also request
   * a redraw GTK-side
   */
  priv->queue_redraw_id =
    g_signal_connect (priv->stage,
                      "queue-redraw", G_CALLBACK (on_stage_queue_redraw),
                      embed);
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
  return g_object_new (GTK_CLUTTER_TYPE_EMBED, NULL);
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
  g_return_val_if_fail (GTK_CLUTTER_IS_EMBED (embed), NULL);

  return embed->priv->stage;
}
