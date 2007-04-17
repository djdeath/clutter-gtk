/*
 * Clutter-Gtk
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
 * #GtkClutter is a GTK+ widget, derived from #GtkSocket that contains a
 * #ClutterStage, allowing it to be used in a GTK+ based program like any 
 * normal GTK+ widget.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gdk/gdkx.h>

#include <gtk/gtkwidget.h>

#include <clutter/clutter-main.h>
#include <clutter/clutter-stage.h>
#include <clutter/clutter-glx.h>

#include "clutter-gtk.h"

#define GTK_CLUTTER_GET_PRIVATE(obj) \
(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_CLUTTER, GtkClutterPrivate))

struct _GtkClutterPrivate
{
  ClutterActor *stage;

  guint is_embedded : 1;
};

enum
{
  PROP_0,

  PROP_EMBEDDED
};

G_DEFINE_TYPE (GtkClutter, gtk_clutter, GTK_TYPE_SOCKET);

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

  GTK_WIDGET_CLASS (gtk_clutter_parent_class)->size_allocate (widget, 
							      allocation);
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
gtk_clutter_map (GtkWidget *widget)
{
  GtkSocket *socket = GTK_SOCKET (widget);
  GtkClutterPrivate *priv = GTK_CLUTTER (widget)->priv;
  ClutterStage *stage = CLUTTER_STAGE (priv->stage);

  if (!priv->is_embedded)
    {
      g_object_ref (widget);

      gtk_socket_add_id (socket, clutter_glx_get_stage_window (stage)); 
      priv->is_embedded = TRUE;

      g_object_notify (G_OBJECT (widget), "embedded");
      g_object_unref (widget);
    }

  GTK_WIDGET_CLASS (gtk_clutter_parent_class)->map (widget);
}

static void
gtk_clutter_get_property (GObject    *gobject,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GtkClutter *gtk_clutter = GTK_CLUTTER (gobject);

  switch (prop_id)
    {
    case PROP_EMBEDDED:
      g_value_set_boolean (value, gtk_clutter->priv->is_embedded);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
gtk_clutter_class_init (GtkClutterClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gobject_class->get_property = gtk_clutter_get_property;

  object_class->destroy = gtk_clutter_destroy;

  widget_class->size_request = gtk_clutter_size_request;
  widget_class->size_allocate = gtk_clutter_size_allocate;
  widget_class->map = gtk_clutter_map;

  g_object_class_install_property (gobject_class,
                                   PROP_EMBEDDED,
                                   g_param_spec_boolean ("embedded",
                                                         "Embedded",
                                                         "Whether the stage has been successfully embedded",
                                                         FALSE,
                                                         G_PARAM_READABLE));

  g_type_class_add_private (gobject_class, sizeof (GtkClutterPrivate));
}

static void
gtk_clutter_init (GtkClutter *clutter)
{
  GtkClutterPrivate *priv;

  clutter->priv = priv = GTK_CLUTTER_GET_PRIVATE (clutter);

  gtk_widget_set_double_buffered (GTK_WIDGET (clutter), FALSE);

  priv->stage = clutter_stage_get_default ();
  priv->is_embedded = FALSE;
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
