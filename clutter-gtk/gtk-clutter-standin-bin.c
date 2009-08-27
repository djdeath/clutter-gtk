/* gtk-clutter-actor.c: a ClutterContainer used by GtkClutterStandin
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gtk-clutter-standin-bin.h"
#include "gtk-clutter-standin.h"

#include <glib-object.h>

static void clutter_container_iface_init (ClutterContainerIface *iface);

G_DEFINE_TYPE_WITH_CODE (GtkClutterStandinBin,
			 gtk_clutter_standin_bin,
			 CLUTTER_TYPE_GROUP,
			 G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_CONTAINER,
						clutter_container_iface_init));

static void
gtk_clutter_standin_bin_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtk_clutter_standin_bin_parent_class)->finalize (object);
}

static void
gtk_clutter_standin_bin_get_preferred_width (ClutterActor *actor,
                                             gfloat        for_height,
                                             gfloat       *min_width_p,
                                             gfloat       *natural_width_p)
{
    GtkWidget *standin;

    CLUTTER_ACTOR_CLASS (gtk_clutter_standin_bin_parent_class)->get_preferred_width (
            actor, for_height, min_width_p, natural_width_p);

    /* determine if this container has changed size compared to the current
     * requisition of the GtkClutterStandin it belongs to, and if required,
     * queue a resize */
    standin = GTK_CLUTTER_STANDIN_BIN (actor)->standin;
    if (*natural_width_p != standin->requisition.width)
    {
        g_print ("width changed (%g)\n", *natural_width_p);
        g_object_set (actor, "natural-width-set", FALSE, NULL);
        gtk_widget_queue_resize (standin);
    }
}

static void
gtk_clutter_standin_bin_get_preferred_height (ClutterActor *actor,
                                              gfloat        for_width,
                                              gfloat       *min_height_p,
                                              gfloat       *natural_height_p)
{
    GtkWidget *standin;

    CLUTTER_ACTOR_CLASS (gtk_clutter_standin_bin_parent_class)->get_preferred_height (
            actor, for_width, min_height_p, natural_height_p);

    /* determine if this container has changed size compared to the current
     * requisition of the GtkClutterStandin it belongs to, and if required,
     * queue a resize */
    standin = GTK_CLUTTER_STANDIN_BIN (actor)->standin;
    if (*natural_height_p != standin->requisition.height)
    {
        g_print ("height changed\n");
        g_object_set (actor, "natural-height-set", FALSE, NULL);
        gtk_widget_queue_resize (standin);
    }
}

static void
gtk_clutter_standin_bin_class_init (GtkClutterStandinBinClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  actor_class->get_preferred_width  = gtk_clutter_standin_bin_get_preferred_width;
  actor_class->get_preferred_height = gtk_clutter_standin_bin_get_preferred_height;

  gobject_class->finalize = gtk_clutter_standin_bin_finalize;
}

static void
gtk_clutter_standin_bin_init (GtkClutterStandinBin *self)
{
}

static void
gtk_clutter_standin_bin_add (ClutterContainer *self,
                             ClutterActor     *actor)
{
    ClutterContainerIface *parent_iface;

    g_return_if_fail (GTK_CLUTTER_STANDIN_BIN (self)->child == NULL);

    GTK_CLUTTER_STANDIN_BIN (self)->child = actor;

    parent_iface = g_type_interface_peek_parent (
            CLUTTER_CONTAINER_GET_IFACE (self));
    parent_iface->add (self, actor);
}

static void
gtk_clutter_standin_bin_remove (ClutterContainer *self,
                                ClutterActor     *actor)
{
    ClutterContainerIface *parent_iface;

    g_return_if_fail (GTK_CLUTTER_STANDIN_BIN (self)->child != actor);

    parent_iface = g_type_interface_peek_parent (
            CLUTTER_CONTAINER_GET_IFACE (self));
    parent_iface->remove (self, actor);

    GTK_CLUTTER_STANDIN_BIN (self)->child = NULL;
}

static void
clutter_container_iface_init (ClutterContainerIface *iface)
{
  iface->add    = gtk_clutter_standin_bin_add;
  iface->remove = gtk_clutter_standin_bin_remove;
}
