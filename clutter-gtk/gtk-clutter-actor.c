/* gtk-clutter-actor.h: Gtk widget ClutterActor
 *
 * Copyright (C) 2009 Red Hat, Inc
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
 *   Alexander Larsson <alexl@redhat.com>
 */

/**
 * SECTION:gtk-clutter-actor
 * @short_description: actor for embedding a Widget in a Clutter stage
 *
 *
 * Since: 0.9
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gtk-clutter-actor.h"
#include "gtk-clutter-offscreen.h"

#include <glib-object.h>

#include <gdk/gdk.h>

#define USE_GLX_TEXTURE 1

#if defined(HAVE_CLUTTER_GTK_X11)

#include <clutter/x11/clutter-x11.h>
#include <clutter/glx/clutter-glx-texture-pixmap.h>
#include <gdk/gdkx.h>

#elif defined(HAVE_CLUTTER_GTK_WIN32)

#include <clutter/clutter-win32.h>
#include <gdk/gdkwin32.h>

#endif /* HAVE_CLUTTER_GTK_{X11,WIN32} */

static void clutter_container_iface_init (ClutterContainerIface *iface);

G_DEFINE_TYPE_WITH_CODE (GtkClutterActor,
			 gtk_clutter_actor,
			 CLUTTER_TYPE_ACTOR,
			 G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_CONTAINER,
						clutter_container_iface_init));

#define GTK_CLUTTER_ACTOR_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_CLUTTER_TYPE_ACTOR, GtkClutterActorPrivate))

struct _GtkClutterActorPrivate
{
  GtkWidget *widget;
  GtkWidget *embed;
  GdkPixmap *pixmap;
  ClutterActor *texture;
};

static void
gtk_clutter_actor_finalize (GObject *object)
{
  G_OBJECT_CLASS (gtk_clutter_actor_parent_class)->finalize (object);
}

static void
gtk_clutter_actor_realize (ClutterActor *actor)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  ClutterActor *stage;
  GtkWidget *new_embed;

  new_embed = NULL;
  stage = clutter_actor_get_stage (actor);
  clutter->priv->embed = g_object_get_data (G_OBJECT (stage), "gtk-clutter-embed");
  gtk_container_add (GTK_CONTAINER (clutter->priv->embed),
		     clutter->priv->widget);

  gtk_widget_realize (clutter->priv->widget);
  clutter->priv->pixmap = gdk_offscreen_window_get_pixmap (clutter->priv->widget->window);
  g_object_ref (clutter->priv->pixmap);

  clutter_x11_texture_pixmap_set_pixmap (CLUTTER_X11_TEXTURE_PIXMAP (clutter->priv->texture),
					 GDK_PIXMAP_XID (clutter->priv->pixmap));
}

static void
gtk_clutter_actor_unrealize (ClutterActor *actor)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);

  gtk_widget_unrealize (clutter->priv->widget);
  g_object_unref (clutter->priv->pixmap);
  clutter->priv->pixmap = NULL;

  gtk_container_remove (GTK_CONTAINER (clutter->priv->embed),
			clutter->priv->widget);
  clutter->priv->embed = NULL;
}

static void
gtk_clutter_actor_get_preferred_width (ClutterActor *actor,
				       gfloat        for_height,
				       gfloat       *min_width_p,
				       gfloat       *natural_width_p)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  GtkRequisition requisition;

  gtk_widget_size_request (clutter->priv->widget, &requisition);
  *min_width_p = *natural_width_p = requisition.width;
}

static void
gtk_clutter_actor_get_preferred_height (ClutterActor *actor,
					gfloat        for_width,
					gfloat       *min_height_p,
					gfloat       *natural_height_p)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  GtkRequisition requisition;

  gtk_widget_size_request (clutter->priv->widget, &requisition);
  *min_height_p = *natural_height_p = requisition.height;
}

static void
gtk_clutter_actor_allocate (ClutterActor           *actor,
			    const ClutterActorBox  *box,
			    ClutterAllocationFlags  flags)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  GtkAllocation child_allocation;
  GdkPixmap *pixmap;
  ClutterActorBox child_box;

  CLUTTER_ACTOR_CLASS (gtk_clutter_actor_parent_class)->allocate (actor, box, flags);

  child_allocation.x = 0;
  child_allocation.y = 0;
  child_allocation.width = box->x2 - box->x1;
  child_allocation.height = box->y2 - box->y1;

  gtk_widget_size_allocate (clutter->priv->widget, &child_allocation);

  if (CLUTTER_ACTOR_IS_REALIZED (actor))
    {
      /* The former size allocate may have queued exposed, we then need to
	 process them immediately, since we will paint the pixmap when this
	 returns (as size allocation is done from clutter_redraw which is
	 called from gtk_clutter_expose_event(). If we don't do this we
	 may see an intermediate state of the pixmap, causing flicker */
      gdk_window_process_updates (clutter->priv->widget->window, TRUE);

      pixmap = gdk_offscreen_window_get_pixmap (clutter->priv->widget->window);
      if (pixmap != clutter->priv->pixmap)
	{
	  g_object_unref (clutter->priv->pixmap);
	  clutter->priv->pixmap = pixmap;
	  g_object_ref (clutter->priv->pixmap);

	  clutter_x11_texture_pixmap_set_pixmap (CLUTTER_X11_TEXTURE_PIXMAP (clutter->priv->texture),
						 GDK_PIXMAP_XID (clutter->priv->pixmap));
	}
    }

  child_box.x1 = 0;
  child_box.y1 = 0;
  child_box.x2 = box->x2 - box->x1;
  child_box.y2 = box->y2 - box->y1;
  clutter_actor_allocate (clutter->priv->texture, &child_box, flags);
}

static void
gtk_clutter_actor_paint (ClutterActor *actor)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  clutter_actor_paint (clutter->priv->texture);
}

static void
gtk_clutter_actor_class_init (GtkClutterActorClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GtkClutterActorPrivate));

  actor_class->paint = gtk_clutter_actor_paint;
  actor_class->realize = gtk_clutter_actor_realize;
  actor_class->unrealize = gtk_clutter_actor_unrealize;

  actor_class->get_preferred_width  = gtk_clutter_actor_get_preferred_width;
  actor_class->get_preferred_height = gtk_clutter_actor_get_preferred_height;
  actor_class->allocate             = gtk_clutter_actor_allocate;

  gobject_class->finalize = gtk_clutter_actor_finalize;

}

static void
gtk_clutter_actor_init (GtkClutterActor *actor)
{
  GtkClutterActorPrivate *priv;

  actor->priv = priv = GTK_CLUTTER_ACTOR_GET_PRIVATE (actor);

  priv->widget = gtk_clutter_offscreen_new (CLUTTER_ACTOR (actor));
  gtk_widget_show (priv->widget);
#if USE_GLX_TEXTURE
  priv->texture = clutter_glx_texture_pixmap_new ();
#else
  priv->texture = clutter_x11_texture_pixmap_new ();
#endif
  clutter_actor_set_parent (priv->texture, CLUTTER_ACTOR (actor));
  clutter_actor_show (priv->texture);
}

static void
gtk_clutter_actor_add (ClutterContainer *container,
		       ClutterActor     *actor)
{
  g_warning ("Can't add children to GtkClutterActor");
}

static void
gtk_clutter_actor_remove (ClutterContainer *container,
			  ClutterActor     *actor)
{
}

static void
gtk_clutter_actor_foreach (ClutterContainer *container,
			   ClutterCallback   callback,
			   gpointer          user_data)
{
}

static void
gtk_clutter_actor_foreach_with_internals (ClutterContainer *container,
					  ClutterCallback   callback,
					  gpointer          user_data)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (container);

  callback (clutter->priv->texture, user_data);
}

GtkWidget *
_gtk_clutter_actor_get_embed (GtkClutterActor *actor)
{
  return actor->priv->embed;
}

void
_gtk_clutter_actor_update (GtkClutterActor *actor,
			   gint             x,
			   gint             y,
			   gint             width,
			   gint             height)
{
  clutter_x11_texture_pixmap_update_area (CLUTTER_X11_TEXTURE_PIXMAP (actor->priv->texture),
					  x, y, width, height);
  clutter_actor_queue_redraw (CLUTTER_ACTOR (actor));
}

static void
clutter_container_iface_init (ClutterContainerIface *iface)
{
  iface->add = gtk_clutter_actor_add;
  iface->remove = gtk_clutter_actor_remove;
  iface->foreach = gtk_clutter_actor_foreach;
  iface->foreach_with_internals = gtk_clutter_actor_foreach_with_internals;
}

/**
 * gtk_clutter_actor_new:
 *
 * Creates a new #GtkClutterActor widget. This widget can be
 * used to embed a Gtk widget into a clutter scene.
 *
 * Return value: the newly created #GtkClutterActor
 *
 * Since: 0.9
 */
ClutterActor *
gtk_clutter_actor_new (void)
{
  return g_object_new (GTK_CLUTTER_TYPE_ACTOR, NULL);
}


void
gtk_clutter_actor_set_recieves_events (GtkClutterActor *actor,
				       gboolean         recieves_events)
{
  gtk_clutter_offscreen_set_active (GTK_CLUTTER_OFFSCREEN (actor->priv->widget),
				    recieves_events);
}

gboolean
gtk_clutter_actor_get_recieves_events (GtkClutterActor *actor)
{
  return GTK_CLUTTER_OFFSCREEN (actor->priv->widget)->active;
}

/**
 * gtk_clutter_actor_get_widget:
 * @actor: a #GtkClutterActor
 *
 *
 * Since: 0.9
 */
GtkWidget *
gtk_clutter_actor_get_widget (GtkClutterActor *actor)
{
  g_return_val_if_fail (GTK_CLUTTER_IS_ACTOR (actor), NULL);

  return actor->priv->widget;
}
