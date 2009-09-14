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
 *   Danielle Madeley <danielle.madeley@collabora.co.uk>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gtk-clutter-standin-bin.h"
#include "gtk-clutter-standin.h"

#include <glib-object.h>
#include <math.h>

static void clutter_container_iface_init (ClutterContainerIface *iface);

G_DEFINE_TYPE_WITH_CODE (GtkClutterStandinBin,
                         gtk_clutter_standin_bin,
                         CLUTTER_TYPE_ACTOR,
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

  clutter_actor_get_preferred_width (
          GTK_CLUTTER_STANDIN_BIN (actor)->child,
          for_height, min_width_p, natural_width_p);

  /* determine if this container has changed size compared to the current
   * requisition of the GtkClutterStandin it belongs to, and if required,
   * queue a resize */
  standin = GTK_CLUTTER_STANDIN_BIN (actor)->standin;
  if (*natural_width_p != standin->requisition.width)
    {
      clutter_actor_set_width (actor, -1);
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

  clutter_actor_get_preferred_height (
          GTK_CLUTTER_STANDIN_BIN (actor)->child,
          for_width, min_height_p, natural_height_p);

  /* determine if this container has changed size compared to the current
   * requisition of the GtkClutterStandin it belongs to, and if required,
   * queue a resize */
  standin = GTK_CLUTTER_STANDIN_BIN (actor)->standin;
  if (*natural_height_p != standin->requisition.height)
    {
      clutter_actor_set_height (actor, -1);
      gtk_widget_queue_resize (standin);
    }
}

static void
gtk_clutter_standin_bin_allocate (ClutterActor          *self,
                                  const ClutterActorBox *box,
                                  ClutterAllocationFlags flags)
{
  /* we only want to accept allocations from GTK+, thus allocation is
   * done by calling gtk_clutter_standin_bin_gtk_allocate() */
}

void
gtk_clutter_standin_bin_gtk_size_request (GtkClutterStandinBin *self,
                                          GtkRequisition       *requisition)
{
  ClutterRequestMode request_mode;
  float width, height;

  g_object_get (self, "request-mode", &request_mode, NULL);

  if (request_mode == CLUTTER_REQUEST_HEIGHT_FOR_WIDTH)
    {
      gtk_clutter_standin_bin_get_preferred_width (CLUTTER_ACTOR (self),
                                         -1, NULL, &width);

      gtk_clutter_standin_bin_get_preferred_height (CLUTTER_ACTOR (self),
                                          width, NULL, &height);
    }
  else
    {
      gtk_clutter_standin_bin_get_preferred_height (CLUTTER_ACTOR (self),
                                          -1, NULL, &height);

      gtk_clutter_standin_bin_get_preferred_width (CLUTTER_ACTOR (self),
                                         height, NULL, &width);
    }

  requisition->width = ceil (width);
  requisition->height = ceil (height);
}

void
gtk_clutter_standin_bin_gtk_size_allocate (GtkClutterStandinBin  *self,
                                           GtkAllocation         *allocation)
{
  ClutterRequestMode request_mode;
  ClutterActorBox box;
  float width, height;
  float min_width, natural_width;
  float min_height, natural_height;

  g_object_get (self, "request-mode", &request_mode, NULL);

  if (request_mode == CLUTTER_REQUEST_HEIGHT_FOR_WIDTH)
    {
      gtk_clutter_standin_bin_get_preferred_width (CLUTTER_ACTOR (self),
                                         (float) allocation->height,
                                         &min_width,
                                         &natural_width);
      width = CLAMP (natural_width, min_width, (float) allocation->width);

      gtk_clutter_standin_bin_get_preferred_height (CLUTTER_ACTOR (self),
                                          width,
                                          &min_height,
                                          &natural_height);
      height = CLAMP (natural_height, min_height, (float) allocation->height);
    }
  else
    {
      gtk_clutter_standin_bin_get_preferred_height (CLUTTER_ACTOR (self),
                                          (float) allocation->width,
                                          &min_height,
                                          &natural_height);
      height = CLAMP (natural_height, min_height, (float) allocation->height);

      gtk_clutter_standin_bin_get_preferred_width (CLUTTER_ACTOR (self),
                                         height,
                                         &min_width,
                                         &natural_width);
      width = CLAMP (natural_width, min_width, (float) allocation->width);
    }

  box.x1 = (float) allocation->x;
  box.y1 = (float) allocation->y;
  box.x2 = box.x1 + width;
  box.y2 = box.y1 + height;

  clutter_actor_allocate (GTK_CLUTTER_STANDIN_BIN (self)->child,
                          &box, CLUTTER_ALLOCATION_NONE);
}

static void
gtk_clutter_standin_bin_paint (ClutterActor *self)
{
  clutter_actor_paint (GTK_CLUTTER_STANDIN_BIN (self)->child);
}

static void
gtk_clutter_standin_bin_pick (ClutterActor       *self,
                              const ClutterColor *color)
{
  CLUTTER_ACTOR_CLASS (gtk_clutter_standin_bin_parent_class)->pick (self,
                                                                    color);
  clutter_actor_paint (GTK_CLUTTER_STANDIN_BIN (self)->child);
}

static void
gtk_clutter_standin_bin_show (ClutterActor *actor)
{
  clutter_container_foreach (CLUTTER_CONTAINER (actor),
                             CLUTTER_CALLBACK (clutter_actor_show),
                             NULL);
  CLUTTER_ACTOR_CLASS (gtk_clutter_standin_bin_parent_class)->show (actor);
}

static void
gtk_clutter_standin_bin_show_all (ClutterActor *actor)
{
  clutter_container_foreach (CLUTTER_CONTAINER (actor),
                             CLUTTER_CALLBACK (clutter_actor_show_all),
                             NULL);
  CLUTTER_ACTOR_CLASS (gtk_clutter_standin_bin_parent_class)->show (actor);
}

static void
gtk_clutter_standin_bin_hide (ClutterActor *actor)
{
  clutter_actor_hide (actor);
  clutter_container_foreach (CLUTTER_CONTAINER (actor),
                             CLUTTER_CALLBACK (clutter_actor_hide),
                             NULL);
}

static void
gtk_clutter_standin_bin_hide_all (ClutterActor *actor)
{
  clutter_actor_hide (actor);
  clutter_container_foreach (CLUTTER_CONTAINER (actor),
                             CLUTTER_CALLBACK (clutter_actor_hide_all),
                             NULL);
}

static void
gtk_clutter_standin_bin_class_init (GtkClutterStandinBinClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  actor_class->get_preferred_width  = gtk_clutter_standin_bin_get_preferred_width;
  actor_class->get_preferred_height = gtk_clutter_standin_bin_get_preferred_height;
  actor_class->allocate = gtk_clutter_standin_bin_allocate;
  actor_class->paint    = gtk_clutter_standin_bin_paint;
  actor_class->pick     = gtk_clutter_standin_bin_pick;
  actor_class->show     = gtk_clutter_standin_bin_show;
  actor_class->show_all = gtk_clutter_standin_bin_show_all;
  actor_class->hide     = gtk_clutter_standin_bin_hide;
  actor_class->hide_all = gtk_clutter_standin_bin_hide_all;

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
  g_return_if_fail (GTK_CLUTTER_STANDIN_BIN (self)->child == NULL);

  g_object_ref (actor);

  GTK_CLUTTER_STANDIN_BIN (self)->child = actor;
  clutter_actor_set_parent (actor, CLUTTER_ACTOR (self));

  clutter_actor_queue_relayout (CLUTTER_ACTOR (self));

  g_object_unref (actor);
}

static void
gtk_clutter_standin_bin_remove (ClutterContainer *self,
                                ClutterActor     *actor)
{
  g_return_if_fail (GTK_CLUTTER_STANDIN_BIN (self)->child != actor);

  g_object_ref (actor);

  GTK_CLUTTER_STANDIN_BIN (self)->child = NULL;
  clutter_actor_unparent (actor);

  clutter_actor_queue_relayout (CLUTTER_ACTOR (self));

  g_object_unref (actor);
}

static void
gtk_clutter_standin_bin_foreach (ClutterContainer *self,
                                 ClutterCallback   callback,
                                 gpointer          user_data)
{
  callback (GTK_CLUTTER_STANDIN_BIN (self)->child, user_data);
}

static void
clutter_container_iface_init (ClutterContainerIface *iface)
{
  iface->add     = gtk_clutter_standin_bin_add;
  iface->remove  = gtk_clutter_standin_bin_remove;
  iface->foreach = gtk_clutter_standin_bin_foreach;
}
