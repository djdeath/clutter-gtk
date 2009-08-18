/* gtk-clutter-window.c: GtkWindow which provides a hidden ClutterStage
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
 *   Davyd Madeley <davyd.madeley@collabora.co.uk>
 */

/**
 * SECTION:gtk-clutter-window
 * @short_description: a #GtkWindow that embeds its contents onto a #ClutterStage
 *
 *
 * Since: 1.0
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gtk-clutter-window.h"
#include "gtk-clutter-actor.h"
#include "gtk-clutter-embed.h"

#include <glib-object.h>

G_DEFINE_TYPE (GtkClutterWindow,
               gtk_clutter_window,
               GTK_TYPE_WINDOW);

#define GTK_CLUTTER_WINDOW_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_CLUTTER_TYPE_WINDOW, GtkClutterWindowPrivate))

struct _GtkClutterWindowPrivate
{
  GtkWidget *embed;
  ClutterActor *actor;
};

static void
gtk_clutter_window_finalize (GObject *self)
{
  G_OBJECT_CLASS (gtk_clutter_window_parent_class)->finalize (self);
}

static void
gtk_clutter_window_size_request (GtkWidget      *self,
                                 GtkRequisition *requisition)
{
    GtkClutterWindowPrivate *priv;
    GtkWidget *bin;

    g_return_if_fail (GTK_CLUTTER_IS_WINDOW (self));
    priv = GTK_CLUTTER_WINDOW (self)->priv;

    bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (priv->actor));

    /* find out what the preferred size of the bin contents are, since we
     * can't ask Clutter for some reason (it always returns the allocated
     * size -- why?). This means things like any scaling applied to the actor
     * won't make the window change size (feature?) */
    gtk_widget_size_request (gtk_bin_get_child (GTK_BIN (bin)), requisition);
}

static void
gtk_clutter_window_add (GtkContainer *self,
                        GtkWidget    *widget)
{
    GtkClutterWindowPrivate *priv;
    GtkWidget *bin;

    g_return_if_fail (GTK_CLUTTER_IS_WINDOW (self));
    priv = GTK_CLUTTER_WINDOW (self)->priv;

    bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (priv->actor));
    GTK_CONTAINER_GET_CLASS (bin)->add (GTK_CONTAINER (bin), widget);
}

static void
gtk_clutter_window_remove (GtkContainer *self,
                           GtkWidget    *widget)
{
    GtkClutterWindowPrivate *priv;
    GtkWidget *bin;

    g_return_if_fail (GTK_CLUTTER_IS_WINDOW (self));
    priv = GTK_CLUTTER_WINDOW (self)->priv;

    bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (priv->actor));
    GTK_CONTAINER_GET_CLASS (bin)->remove (GTK_CONTAINER (bin), widget);
}

static void
gtk_clutter_window_forall (GtkContainer *self,
                           gboolean      include_internals,
                           GtkCallback   callback,
                           gpointer      callback_data)
{
    GtkClutterWindowPrivate *priv;
    GtkWidget *bin;

    g_return_if_fail (GTK_CLUTTER_IS_WINDOW (self));
    priv = GTK_CLUTTER_WINDOW (self)->priv;

    /* this is particularly dodgy -- if we have asked to include_internals
     * let's only return the internals, on the assumption that when events
     * are sent to those internals, the child container will be iterated;
     * otherwise, we don't want anyone to know about the container, so we
     * return the contents of the bin */
    if (include_internals)
    {
        GTK_CONTAINER_CLASS (gtk_clutter_window_parent_class)->forall (self,
                include_internals, callback, callback_data);
    }
    else
    {
        bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (priv->actor));
        GTK_CONTAINER_GET_CLASS (bin)->forall (GTK_CONTAINER (bin),
                include_internals, callback, callback_data);
    }
}

static void
gtk_clutter_window_set_focus_child (GtkContainer *self,
                                    GtkWidget    *widget)
{
    GtkClutterWindowPrivate *priv;
    GtkWidget *bin;

    g_return_if_fail (GTK_CLUTTER_IS_WINDOW (self));
    priv = GTK_CLUTTER_WINDOW (self)->priv;

    bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (priv->actor));
    GTK_CONTAINER_GET_CLASS (bin)->set_focus_child (GTK_CONTAINER (bin), widget);
}

static GType
gtk_clutter_window_child_type (GtkContainer *self)
{
    GtkClutterWindowPrivate *priv;
    GtkWidget *bin;

    g_return_if_fail (GTK_CLUTTER_IS_WINDOW (self));
    priv = GTK_CLUTTER_WINDOW (self)->priv;

    bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (priv->actor));
    return GTK_CONTAINER_GET_CLASS (bin)->child_type (GTK_CONTAINER (bin));
}

static char *
gtk_clutter_window_composite_name (GtkContainer *self,
                                   GtkWidget    *widget)
{
    GtkClutterWindowPrivate *priv;
    GtkWidget *bin;

    g_return_if_fail (GTK_CLUTTER_IS_WINDOW (self));
    priv = GTK_CLUTTER_WINDOW (self)->priv;

    bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (priv->actor));
    return GTK_CONTAINER_GET_CLASS (bin)->composite_name (GTK_CONTAINER (bin), widget);
}

static void
gtk_clutter_window_set_child_property (GtkContainer *self,
                                       GtkWidget    *widget,
                                       guint         property_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
    GtkClutterWindowPrivate *priv;
    GtkWidget *bin;

    g_return_if_fail (GTK_CLUTTER_IS_WINDOW (self));
    priv = GTK_CLUTTER_WINDOW (self)->priv;

    bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (priv->actor));
    GTK_CONTAINER_GET_CLASS (bin)->set_child_property (GTK_CONTAINER (bin),
            widget, property_id, value, pspec);
}

static void
gtk_clutter_window_get_child_property (GtkContainer *self,
                                       GtkWidget    *widget,
                                       guint         property_id,
                                       GValue       *value,
                                       GParamSpec   *pspec)
{
    GtkClutterWindowPrivate *priv;
    GtkWidget *bin;

    g_return_if_fail (GTK_CLUTTER_IS_WINDOW (self));
    priv = GTK_CLUTTER_WINDOW (self)->priv;

    bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (priv->actor));
    GTK_CONTAINER_GET_CLASS (bin)->get_child_property (GTK_CONTAINER (bin),
            widget, property_id, value, pspec);
}

static void
gtk_clutter_window_class_init (GtkClutterWindowClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtkClutterWindowPrivate));

  gobject_class->finalize = gtk_clutter_window_finalize;

  widget_class->size_request = gtk_clutter_window_size_request;

  /* connect all of the container methods up to our bin */
  container_class->add             = gtk_clutter_window_add;
  container_class->remove          = gtk_clutter_window_remove;
  // container_class->check_resize    = gtk_clutter_window_check_resize;
  container_class->forall          = gtk_clutter_window_forall;
  container_class->set_focus_child = gtk_clutter_window_set_focus_child;
  container_class->child_type      = gtk_clutter_window_child_type;
  container_class->composite_name  = gtk_clutter_window_composite_name;
  container_class->set_child_property = gtk_clutter_window_set_child_property;
  container_class->get_child_property = gtk_clutter_window_get_child_property;
}

static void
gtk_clutter_window_stage_dimensions_changed (GtkClutterWindow *self,
                                             GParamSpec      *pspec,
                                             ClutterActor    *stage)
{
    GtkClutterWindowPrivate *priv = self->priv;
    float w, h;

    /* push these dimensions to the actor */
    clutter_actor_get_size (stage, &w, &h);
    clutter_actor_set_size (priv->actor, w, h);
}

static void
gtk_clutter_window_init (GtkClutterWindow *self)
{
  GtkClutterWindowPrivate *priv;
  ClutterActor *stage;

  self->priv = priv = GTK_CLUTTER_WINDOW_GET_PRIVATE (self);

  priv->embed = gtk_clutter_embed_new ();
  stage = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (priv->embed));

  GTK_CONTAINER_CLASS (gtk_clutter_window_parent_class)->add (
          GTK_CONTAINER (self), priv->embed);
  gtk_widget_show (priv->embed);

  priv->actor = gtk_clutter_actor_new ();
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), priv->actor);

  g_signal_connect_swapped (stage, "notify::width",
          G_CALLBACK (gtk_clutter_window_stage_dimensions_changed), self);
  g_signal_connect_swapped (stage, "notify::height",
          G_CALLBACK (gtk_clutter_window_stage_dimensions_changed), self);
}

/**
 * gtk_clutter_window_new:
 *
 * Creates a new #GtkClutterWindow widget. This window provides a hidden
 * ClutterStage on which the child GtkWidgets are placed. This allows other
 * ClutterActors to also be placed on the stage.
 *
 * Return value: the newly created #GtkClutterWindow
 *
 * Since: 1.0
 */
GtkWidget *
gtk_clutter_window_new (void)
{
    return g_object_new (GTK_CLUTTER_TYPE_WINDOW, NULL);
}

/**
 * gtk_clutter_window_get_stage:
 * @self: the #GtkClutterWindow
 *
 * Retrieves the #ClutterStage that this window is mounting the GTK+ widget
 * tree onto.
 *
 * Use this function if you wish to add other actors to the #ClutterStage.
 *
 * Return value: the window's #ClutterStage
 *
 * Since: 1.0
 */
ClutterActor *
gtk_clutter_window_get_stage (GtkClutterWindow *self)
{
    GtkClutterWindowPrivate *priv;

    g_return_if_fail (GTK_CLUTTER_IS_WINDOW (self));

    priv = self->priv;

    return gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (priv->embed));
}
