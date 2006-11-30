/*
 * GTK-Clutter.
 *
 * GTK+ widget for Clutter.
 *
 * Authored By Iain Holmes  <iain@openedhand.com>
 *
 * Copyright (C) 2006 OpenedHand
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
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:gtk-clutter
 * @short_description: GTK+ widget displaying a #ClutterStage.
 *
 * #GtkClutter is a GTK+ widget, derived from #GtkDrawingArea that contains a
 * #ClutterStage, allowing it to be used in a GTK+ based program like any 
 * normal GTK+ widget.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gdk/gdkx.h>

#include <gtk/gtkdrawingarea.h>
#include <gtk/gtkwidget.h>

#include <clutter/clutter-main.h>
#include <clutter/clutter-stage.h>

#include "gtk-clutter.h"

#define GTK_CLUTTER_GET_PRIVATE(obj) \
(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_CLUTTER, GtkClutterPrivate))

struct _GtkClutterPrivate
{
  ClutterActor *stage;
};

G_DEFINE_TYPE (GtkClutter, gtk_clutter, GTK_TYPE_DRAWING_AREA);

static void
gtk_clutter_destroy (GtkObject *object)
{
  GtkClutterPrivate *priv;

  priv = GTK_CLUTTER (object)->priv;

  if (priv->stage)
    {
      clutter_actor_destroy (priv->stage);
      priv->stage = NULL;
    }

  GTK_OBJECT_CLASS (gtk_clutter_parent_class)->destroy (object);
}

static void
gtk_clutter_size_allocate (GtkWidget     *widget,
                           GtkAllocation *allocation)
{
  GtkClutterPrivate *priv = GTK_CLUTTER (widget)->priv;

  clutter_actor_set_size (priv->stage,
                          allocation->width,
                          allocation->height);

  if (CLUTTER_ACTOR_IS_VISIBLE (priv->stage))
    clutter_actor_queue_redraw (priv->stage);
}

static void
gtk_clutter_size_request (GtkWidget      *widget,
                          GtkRequisition *req)
{
  GtkClutterPrivate *priv;

  priv = GTK_CLUTTER (widget)->priv;

  req->width = clutter_actor_get_width (priv->stage);
  req->height = clutter_actor_get_height (priv->stage);
}

static void
gtk_clutter_realize (GtkWidget *widget)
{
  GtkClutterPrivate *priv;
  const XVisualInfo *xvinfo;
  GdkVisual *visual;
  GdkColormap *colormap;

  priv = GTK_CLUTTER (widget)->priv;

  /* We need to use the colormap from the Clutter visual */
  xvinfo = clutter_stage_get_xvisual (CLUTTER_STAGE (priv->stage));
  visual = gdk_x11_screen_lookup_visual (gdk_screen_get_default (),
                                         xvinfo->visualid);
  colormap = gdk_colormap_new (visual, FALSE);
  gtk_widget_set_colormap (widget, colormap);

  /* And turn off double buffering, cos GL doesn't like it */
  gtk_widget_set_double_buffered (widget, FALSE);

  GTK_WIDGET_CLASS (gtk_clutter_parent_class)->realize (widget);

  gdk_window_set_back_pixmap (widget->window, NULL, FALSE);

  priv = GTK_CLUTTER (widget)->priv;

  clutter_stage_set_xwindow_foreign (CLUTTER_STAGE (priv->stage), 
                                     GDK_WINDOW_XID (widget->window));
}

static gboolean
gtk_clutter_expose_event (GtkWidget      *widget,
                          GdkEventExpose *expose)
{
  GtkClutterPrivate *priv = GTK_CLUTTER (widget)->priv;

  clutter_actor_queue_redraw (priv->stage);

  return TRUE;
}

static void
gtk_clutter_class_init (GtkClutterClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->destroy = gtk_clutter_destroy;

  widget_class->size_request = gtk_clutter_size_request;
  widget_class->size_allocate = gtk_clutter_size_allocate;
  widget_class->realize = gtk_clutter_realize;
  widget_class->expose_event = gtk_clutter_expose_event;

  g_type_class_add_private (gobject_class, sizeof (GtkClutterPrivate));
}

static void
gtk_clutter_init (GtkClutter *clutter)
{
  GtkClutterPrivate *priv;

  clutter->priv = priv = GTK_CLUTTER_GET_PRIVATE (clutter);

  gtk_widget_set_double_buffered (GTK_WIDGET (clutter), FALSE);

  priv->stage = clutter_stage_get_default ();
}

/**
 * gtk_clutter_get_stage:
 * @clutter: A #GtkClutter object.
 *
 * Obtains the #ClutterStage associated with this object.
 *
 * Return value: A #ClutterActor.
 */
ClutterActor *
gtk_clutter_get_stage (GtkClutter *clutter)
{
  g_return_val_if_fail (GTK_IS_CLUTTER (clutter), NULL);

  return clutter->priv->stage;
}

GtkWidget *
gtk_clutter_new (void)
{
  return g_object_new (GTK_TYPE_CLUTTER, NULL);
}
