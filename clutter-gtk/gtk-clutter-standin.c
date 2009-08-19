/* gtk-clutter-standin.c: a widget that stands-in for a ClutterActor
 *
 * Copyright (C) 2009 Collabora Ltd.
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
 *   Davyd Madeley  <davyd.madeley@collabora.co.uk>
 */

/**
 * SECTION:gtk-clutter-standin
 * @short_description: a Widget which is a standin for a ClutterActor
 *
 * Since: 1.0
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gtk-clutter-standin.h"
#include "gtk-clutter-offscreen.h"

#include <glib-object.h>
#include <math.h>

G_DEFINE_TYPE (GtkClutterStandin, gtk_clutter_standin, GTK_TYPE_WIDGET);

#define GTK_CLUTTER_STANDIN_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_CLUTTER_TYPE_STANDIN, GtkClutterStandinPrivate))

enum
{
    PROP_0,
    PROP_ACTOR
};

struct _GtkClutterStandinPrivate
{
  ClutterActor *actor;
  gboolean actor_on_stage;
};

static void
gtk_clutter_standin_put_actor_on_stage (GtkClutterStandin *self)
{
    GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (self)->priv;

    if (priv->actor_on_stage) return;

    /* find the stage that this stand-in is placed upon and place our actor
     * upon it */
    GtkWidget *parent = GTK_WIDGET (self);

    while ((parent = gtk_widget_get_parent (parent)))
    {
        if (GTK_CLUTTER_IS_OFFSCREEN (parent)) break;
    }

    if (parent == NULL) return;
    g_return_if_fail (GTK_CLUTTER_IS_OFFSCREEN (parent));

    ClutterActor *stage = clutter_actor_get_stage (
            GTK_CLUTTER_OFFSCREEN (parent)->actor);

    clutter_container_add_actor (CLUTTER_CONTAINER (stage), priv->actor);
    priv->actor_on_stage = TRUE;
}

static void
gtk_clutter_standin_send_configure (GtkClutterStandin *self)
{
  GtkWidget *widget;
  GdkEvent *event = gdk_event_new (GDK_CONFIGURE);

  widget = GTK_WIDGET (self);

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
gtk_clutter_standin_get_property (GObject    *self,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
    GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (self)->priv;

    switch (property_id)
    {
        case PROP_ACTOR:
            g_value_set_object (value, priv->actor);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
            break;
    }
}

static void
gtk_clutter_standin_set_property (GObject      *self,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
    GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (self)->priv;

    switch (property_id)
    {
        case PROP_ACTOR:
            priv->actor = g_value_get_object (value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
            break;
    }
}

static void
gtk_clutter_standin_dispose (GObject *gobject)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (gobject)->priv;

  G_OBJECT_CLASS (gtk_clutter_standin_parent_class)->dispose (gobject);
}

static void
gtk_clutter_standin_show (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  if (GTK_WIDGET_REALIZED (widget))
  {
    clutter_actor_show (priv->actor);
  }

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->show (widget);
}

static void
gtk_clutter_standin_hide (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  /* gtk emits a hide signal during dispose, so it's possible we may
   * have already disposed priv->stage. */
  if (priv->actor)
  {
    clutter_actor_hide (priv->actor);
  }

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->hide (widget);
}

static void
gtk_clutter_standin_realize (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;
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

  attributes.event_mask = gtk_widget_get_events (widget)
                        | GDK_EXPOSURE_MASK;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
                                   &attributes,
                                   attributes_mask);
  gdk_window_set_user_data (widget->window, widget);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);

  gdk_window_set_back_pixmap (widget->window, NULL, FALSE);

  clutter_actor_realize (priv->actor);

  if (GTK_WIDGET_VISIBLE (widget))
  {
    clutter_actor_show (priv->actor);
  }

  gtk_clutter_standin_send_configure (GTK_CLUTTER_STANDIN (widget));
}

static void
gtk_clutter_standin_unrealize (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  clutter_actor_hide (priv->actor);

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->unrealize (widget);
}

static void
gtk_clutter_standin_size_request (GtkWidget      *self,
                                  GtkRequisition *requisition)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (self)->priv;
  float w, h;

  clutter_actor_get_preferred_size (priv->actor, NULL, NULL, &w, &h);
  requisition->width = ceil (w);
  requisition->height = ceil (h);
}

static void
gtk_clutter_standin_size_allocate (GtkWidget     *widget,
                                   GtkAllocation *allocation)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  widget->allocation = *allocation;

  if (GTK_WIDGET_REALIZED (widget))
    {
      gdk_window_move_resize (widget->window,
                              allocation->x, allocation->y,
                              allocation->width, allocation->height);

      gtk_clutter_standin_send_configure (GTK_CLUTTER_STANDIN (widget));
    }

  clutter_actor_set_position (priv->actor,
                              allocation->x,
                              allocation->y);
  clutter_actor_set_size (priv->actor,
                          allocation->width,
                          allocation->height);
}

static void
gtk_clutter_standin_map (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  gtk_clutter_standin_put_actor_on_stage (GTK_CLUTTER_STANDIN (widget));
  clutter_actor_map (priv->actor);

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->map (widget);
}

static void
gtk_clutter_standin_unmap (GtkWidget *widget)
{
  GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (widget)->priv;

  clutter_actor_unmap (priv->actor);

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->unmap (widget);
}

static void
gtk_clutter_standin_style_set (GtkWidget *widget,
                             GtkStyle  *old_style)
{
  GdkScreen *screen;
  GtkSettings *settings;
  gdouble dpi;
  gchar *font_name;
  const cairo_font_options_t *font_options;
  gint double_click_time, double_click_distance;
  ClutterBackend *backend;

  GTK_WIDGET_CLASS (gtk_clutter_standin_parent_class)->style_set (widget,
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
   * a GtkClutterStandin will not look completely alien
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
gtk_clutter_standin_parent_set (GtkWidget *self,
                                GtkWidget *old_parent)
{
    GtkClutterStandinPrivate *priv = GTK_CLUTTER_STANDIN (self)->priv;

    priv->actor_on_stage = FALSE;
    gtk_clutter_standin_put_actor_on_stage (GTK_CLUTTER_STANDIN (self));
}

static void
gtk_clutter_standin_class_init (GtkClutterStandinClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtkClutterStandinPrivate));

  gobject_class->get_property = gtk_clutter_standin_get_property;
  gobject_class->set_property = gtk_clutter_standin_set_property;
  gobject_class->dispose = gtk_clutter_standin_dispose;

  // widget_class->style_set = gtk_clutter_standin_style_set;
  widget_class->size_request = gtk_clutter_standin_size_request;
  widget_class->size_allocate = gtk_clutter_standin_size_allocate;
  widget_class->realize = gtk_clutter_standin_realize;
  widget_class->unrealize = gtk_clutter_standin_unrealize;
  widget_class->show = gtk_clutter_standin_show;
  widget_class->hide = gtk_clutter_standin_hide;
  widget_class->map = gtk_clutter_standin_map;
  widget_class->unmap = gtk_clutter_standin_unmap;
  // widget_class->expose_event = gtk_clutter_standin_expose_event;
  widget_class->parent_set = gtk_clutter_standin_parent_set;

  g_object_class_install_property (gobject_class, PROP_ACTOR,
          g_param_spec_object ("actor",
                               "Actor",
                               "#ClutterActor this widget is standing in for",
                               CLUTTER_TYPE_ACTOR,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void
gtk_clutter_standin_init (GtkClutterStandin *embed)
{
  GtkClutterStandinPrivate *priv;

  embed->priv = priv = GTK_CLUTTER_STANDIN_GET_PRIVATE (embed);
}

/**
 * gtk_clutter_standin_new:
 * @actor: the #ClutterActor to stand-in for
 *
 * Creates a new #GtkClutterStandin widget. This widget is used as a stand-in
 * in the GTK+ widget tree for a widget that is sitting as a separate actor
 * on the #ClutterStage this widget is sat on.
 *
 * This requires the widget tree to be embedded within a #GtkClutterActor.
 *
 * Return value: the newly created #GtkClutterStandin
 *
 * Since: 1.0
 */
GtkWidget *
gtk_clutter_standin_new (ClutterActor *actor)
{
  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), NULL);

  return g_object_new (GTK_CLUTTER_TYPE_STANDIN,
          "actor", actor,
          NULL);
}
