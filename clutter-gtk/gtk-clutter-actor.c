/* gtk-clutter-actor.c: Gtk widget ClutterActor
 *
 * Copyright (C) 2009 Red Hat, Inc
 * Copyright (C) 2010 Intel Corp
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
 *   Danielle Madeley <danielle.madeley@collabora.co.uk>
 *   Emmanuele Bassi <ebassi@linux.intel.com>
 */

/**
 * SECTION:gtk-clutter-actor
 * @title: GtkClutterActor
 * @short_description: actor for embedding a Widget in a Clutter stage
 *
 * #GtkClutterActor is a #ClutterContainer that also allows embedding
 * any #GtkWidget in a Clutter scenegraph.
 *
 * #GtkClutterActor only allows embedding #GtkWidget<!-- -->s when inside
 * the #ClutterStage provided by a #GtkClutterEmbed: it is not possible to
 * use #GtkClutterActor in a #ClutterStage handled by Clutter alone.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gtk-clutter-actor.h"
#include "gtk-clutter-offscreen.h"

#include <math.h>

#include <glib-object.h>

#include <gdk/gdk.h>

#if defined(HAVE_CLUTTER_GTK_X11)

#include <clutter/x11/clutter-x11.h>
#include <gdk/gdkx.h>
#include <cairo/cairo-xlib.h>

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

  GList *children;
};

enum
{
  PROP_0,

  PROP_CONTENTS
};

static void
gtk_clutter_actor_dispose (GObject *object)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (object)->priv;

  if (priv->children != NULL)
    {
      g_list_foreach (priv->children, (GFunc) clutter_actor_destroy, NULL);
      g_list_free (priv->children);

      priv->children = NULL;
    }

  if (priv->widget != NULL)
    {
      gtk_widget_destroy (priv->widget);
      priv->widget = NULL;
    }

  if (priv->texture != NULL)
    {
      clutter_actor_destroy (priv->texture);
      priv->texture = NULL;
    }

  G_OBJECT_CLASS (gtk_clutter_actor_parent_class)->dispose (object);
}

static void
gtk_clutter_actor_realize (ClutterActor *actor)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  GtkClutterActorPrivate *priv = clutter->priv;
  ClutterActor *stage;
  GtkWidget *new_embed;
  gint pixmap_width, pixmap_height;

  new_embed = NULL;
  stage = clutter_actor_get_stage (actor);
  priv->embed = g_object_get_data (G_OBJECT (stage), "gtk-clutter-embed");
  gtk_container_add (GTK_CONTAINER (priv->embed), priv->widget);

  gtk_widget_realize (priv->widget);

  priv->pixmap = gdk_offscreen_window_get_pixmap (gtk_widget_get_window (priv->widget));
  g_object_ref (priv->pixmap);
  gdk_drawable_set_colormap (priv->pixmap, gtk_widget_get_colormap (priv->embed));
  gdk_drawable_get_size (priv->pixmap, &pixmap_width, &pixmap_height);

  clutter_x11_texture_pixmap_set_pixmap (CLUTTER_X11_TEXTURE_PIXMAP (priv->texture),
                                         GDK_PIXMAP_XID (priv->pixmap));
  clutter_actor_set_size (priv->texture, pixmap_width, pixmap_height);
}

static void
gtk_clutter_actor_unrealize (ClutterActor *actor)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  GtkClutterActorPrivate *priv = clutter->priv;

  gtk_widget_unrealize (priv->widget);

  g_object_unref (priv->pixmap);
  priv->pixmap = NULL;

  g_object_ref (priv->widget);
  gtk_container_remove (GTK_CONTAINER (priv->embed), priv->widget);
  priv->embed = NULL;
}

static void
gtk_clutter_actor_get_preferred_width (ClutterActor *actor,
                                       gfloat        for_height,
                                       gfloat       *min_width_p,
                                       gfloat       *natural_width_p)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  GtkClutterActorPrivate *priv = clutter->priv;
  GtkRequisition requisition;

  gtk_widget_size_request (priv->widget, &requisition);

  if (min_width_p)
    *min_width_p = requisition.width;

  if (natural_width_p)
    *natural_width_p = requisition.width;
}

static void
gtk_clutter_actor_get_preferred_height (ClutterActor *actor,
                                        gfloat        for_width,
                                        gfloat       *min_height_p,
                                        gfloat       *natural_height_p)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  GtkClutterActorPrivate *priv = clutter->priv;
  GtkRequisition requisition;

  gtk_widget_size_request (priv->widget, &requisition);

  if (min_height_p)
    *min_height_p = requisition.height;

  if (natural_height_p)
    *natural_height_p = requisition.height;
}

static void
gtk_clutter_actor_allocate (ClutterActor           *actor,
                            const ClutterActorBox  *box,
                            ClutterAllocationFlags  flags)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  GtkClutterActorPrivate *priv = clutter->priv;
  GtkAllocation child_allocation;
  GdkPixmap *pixmap;
  GdkWindow *window;
  ClutterActorBox child_box;
  GList *l;

  CLUTTER_ACTOR_CLASS (gtk_clutter_actor_parent_class)->allocate (actor, box, flags);

  /* allocate the children */
  for (l = priv->children; l != NULL; l = l->next)
    clutter_actor_allocate_preferred_size (l->data, flags);

  child_allocation.x = 0;
  child_allocation.y = 0;
  child_allocation.width = clutter_actor_box_get_width (box);
  child_allocation.height = clutter_actor_box_get_height (box);

  _gtk_clutter_offscreen_set_in_allocation (GTK_CLUTTER_OFFSCREEN (priv->widget), TRUE);

  gtk_widget_size_allocate (priv->widget, &child_allocation);

  if (CLUTTER_ACTOR_IS_REALIZED (actor))
    {
      /* The former size allocate may have queued an expose we then need to
       * process immediately, since we will paint the pixmap when this
       * returns (as size allocation is done from clutter_redraw which is
       * called from gtk_clutter_embed_expose_event(). If we don't do this
       * we may see an intermediate state of the pixmap, causing flicker
       */
      window = gtk_widget_get_window (priv->widget);
      gdk_window_process_updates (window, TRUE);
      pixmap = gdk_offscreen_window_get_pixmap (window);

      if (pixmap != priv->pixmap)
        {
          if (priv->pixmap != NULL)
            g_object_unref (priv->pixmap);

          priv->pixmap = pixmap;
          g_object_ref (priv->pixmap);

          gdk_drawable_set_colormap (priv->pixmap, gtk_widget_get_colormap (clutter->priv->embed));

          clutter_x11_texture_pixmap_set_pixmap (CLUTTER_X11_TEXTURE_PIXMAP (priv->texture),
                                                 GDK_PIXMAP_XID (priv->pixmap));
        }
    }

  _gtk_clutter_offscreen_set_in_allocation (GTK_CLUTTER_OFFSCREEN (priv->widget), FALSE);

  child_box.x1 = 0;
  child_box.y1 = 0;
  child_box.x2 = clutter_actor_box_get_width (box);
  child_box.y2 = clutter_actor_box_get_height (box);
  clutter_actor_allocate (priv->texture, &child_box, flags);
}

static void
gtk_clutter_actor_paint (ClutterActor *actor)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (actor)->priv;

  clutter_actor_paint (priv->texture);

  g_list_foreach (priv->children, (GFunc) clutter_actor_paint, NULL);
}

static void
gtk_clutter_actor_pick (ClutterActor       *actor,
                        const ClutterColor *color)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (actor)->priv;

  CLUTTER_ACTOR_CLASS (gtk_clutter_actor_parent_class)->pick (actor, color);

  g_list_foreach (priv->children, (GFunc) clutter_actor_paint, NULL);
}

static void
gtk_clutter_actor_show (ClutterActor *self)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (self)->priv;
  GtkWidget *widget = gtk_bin_get_child (GTK_BIN (priv->widget));

  CLUTTER_ACTOR_CLASS (gtk_clutter_actor_parent_class)->show (self);

  /* proxy this call through to GTK+ */
  if (widget != NULL)
    gtk_widget_show (widget);

  g_list_foreach (priv->children, (GFunc) clutter_actor_show, NULL);
}

static void
gtk_clutter_actor_show_all (ClutterActor *self)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (self)->priv;

  /* proxy this call through to GTK+ */
  GtkWidget *widget = gtk_bin_get_child (GTK_BIN (priv->widget));
  if (widget != NULL)
    gtk_widget_show_all (widget);

  g_list_foreach (priv->children, (GFunc) clutter_actor_show_all, NULL);

  CLUTTER_ACTOR_CLASS (gtk_clutter_actor_parent_class)->show_all (self);
}

static void
gtk_clutter_actor_hide (ClutterActor *self)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (self)->priv;

  CLUTTER_ACTOR_CLASS (gtk_clutter_actor_parent_class)->hide (self);

  /* proxy this call through to GTK+ */
  GtkWidget *widget = gtk_bin_get_child (GTK_BIN (priv->widget));
  if (widget != NULL)
    gtk_widget_hide (widget);

  g_list_foreach (priv->children, (GFunc) clutter_actor_hide, NULL);
}

static void
gtk_clutter_actor_hide_all (ClutterActor *self)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (self)->priv;

  /* proxy this call through to GTK+ */
  GtkWidget *widget = gtk_bin_get_child (GTK_BIN (priv->widget));
  if (widget != NULL)
    gtk_widget_hide (widget);

  g_list_foreach (priv->children, (GFunc) clutter_actor_hide_all, NULL);

  CLUTTER_ACTOR_CLASS (gtk_clutter_actor_parent_class)->hide_all (self);
}

static void
gtk_clutter_actor_set_contents (GtkClutterActor *actor,
                                GtkWidget       *contents)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (actor)->priv;

  if (contents == gtk_bin_get_child (GTK_BIN (priv->widget)))
    return;

  if (contents != NULL)
    gtk_container_add (GTK_CONTAINER (priv->widget), contents);
  else
    gtk_container_remove (GTK_CONTAINER (priv->widget),
                          gtk_bin_get_child (GTK_BIN (priv->widget)));

  g_object_notify (G_OBJECT (actor), "contents");
}

static void
on_reactive_change (GtkClutterActor *actor)
{
  GtkClutterActorPrivate *priv = actor->priv;
  gboolean is_reactive;

  is_reactive = clutter_actor_get_reactive (CLUTTER_ACTOR (actor));
  _gtk_clutter_offscreen_set_active (GTK_CLUTTER_OFFSCREEN (priv->widget),
                                     is_reactive);
}

static void
gtk_clutter_actor_set_property (GObject       *gobject,
                                guint          prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GtkClutterActor *actor = GTK_CLUTTER_ACTOR (gobject);

  switch (prop_id)
    {
    case PROP_CONTENTS:
      gtk_clutter_actor_set_contents (actor, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
gtk_clutter_actor_get_property (GObject    *gobject,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (gobject)->priv;

  switch (prop_id)
    {
    case PROP_CONTENTS:
      g_value_set_object (value, gtk_bin_get_child (GTK_BIN (priv->widget)));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
gtk_clutter_actor_class_init (GtkClutterActorClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);
  GParamSpec *pspec;

  g_type_class_add_private (klass, sizeof (GtkClutterActorPrivate));

  actor_class->paint     = gtk_clutter_actor_paint;
  actor_class->pick      = gtk_clutter_actor_pick;
  actor_class->realize   = gtk_clutter_actor_realize;
  actor_class->unrealize = gtk_clutter_actor_unrealize;
  actor_class->show      = gtk_clutter_actor_show;
  actor_class->show_all  = gtk_clutter_actor_show_all;
  actor_class->hide      = gtk_clutter_actor_hide;
  actor_class->hide_all  = gtk_clutter_actor_hide_all;

  actor_class->get_preferred_width  = gtk_clutter_actor_get_preferred_width;
  actor_class->get_preferred_height = gtk_clutter_actor_get_preferred_height;
  actor_class->allocate             = gtk_clutter_actor_allocate;

  gobject_class->set_property = gtk_clutter_actor_set_property;
  gobject_class->get_property = gtk_clutter_actor_get_property;
  gobject_class->dispose = gtk_clutter_actor_dispose;

  /**
   * GtkClutterActor:contents:
   *
   * The #GtkWidget to be embedded into the #GtkClutterActor
   */
  pspec = g_param_spec_object ("contents",
                               "Contents",
                               "The widget to be embedded",
                               GTK_TYPE_WIDGET,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (gobject_class, PROP_CONTENTS, pspec);
}

static void
gtk_clutter_actor_init (GtkClutterActor *self)
{
  GtkClutterActorPrivate *priv;
  ClutterActor *actor;

  self->priv = priv = GTK_CLUTTER_ACTOR_GET_PRIVATE (self);
  actor = CLUTTER_ACTOR (self);

  priv->widget = _gtk_clutter_offscreen_new (actor);
  gtk_widget_set_name (priv->widget, "Offscreen Container");
  g_object_ref_sink (priv->widget);
  gtk_widget_show (priv->widget);

  clutter_actor_push_internal (actor);

#if HAVE_CLUTTER_GTK_X11
  priv->texture = clutter_x11_texture_pixmap_new ();

  clutter_texture_set_sync_size (CLUTTER_TEXTURE (priv->texture), FALSE);
  clutter_actor_set_parent (priv->texture, actor);
  clutter_actor_set_name (priv->texture, "Onscreen Texture");
  clutter_actor_show (priv->texture);
#endif

  clutter_actor_pop_internal (actor);

  g_signal_connect (self, "notify::reactive", G_CALLBACK (on_reactive_change), NULL);
}

static void
gtk_clutter_actor_add (ClutterContainer *container,
                       ClutterActor     *actor)
{
  GtkClutterActorPrivate *priv;

  g_return_if_fail (GTK_CLUTTER_IS_ACTOR (container));

  priv = GTK_CLUTTER_ACTOR (container)->priv;

  g_object_ref (actor);

  priv->children = g_list_append (priv->children, actor);
  clutter_actor_set_parent (actor, CLUTTER_ACTOR (container));

  clutter_actor_queue_relayout (CLUTTER_ACTOR (container));

  g_signal_emit_by_name (container, "actor-added");

  g_object_unref (actor);
}

static void
gtk_clutter_actor_remove (ClutterContainer *container,
                          ClutterActor     *actor)
{
  GtkClutterActorPrivate *priv;

  g_return_if_fail (GTK_CLUTTER_IS_ACTOR (container));

  priv = GTK_CLUTTER_ACTOR (container)->priv;

  g_object_ref (actor);

  priv->children = g_list_remove (priv->children, actor);
  clutter_actor_unparent (actor);

  clutter_actor_queue_relayout (CLUTTER_ACTOR (container));

  g_signal_emit_by_name (container, "actor-removed");

  g_object_unref (actor);
}

static void
gtk_clutter_actor_foreach (ClutterContainer *container,
                           ClutterCallback   callback,
                           gpointer          user_data)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (container)->priv;
  GList *l;

  for (l = priv->children; l != NULL; l = l->next)
    callback (l->data, user_data);
}

static void
gtk_clutter_actor_foreach_with_internals (ClutterContainer *container,
                                          ClutterCallback   callback,
                                          gpointer          user_data)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (container)->priv;
  GList *l;

  callback (priv->texture, user_data);

  for (l = priv->children; l != NULL; l = l->next)
    callback (l->data, user_data);
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
  GtkClutterActorPrivate *priv = actor->priv;

#if HAVE_CLUTTER_GTK_X11
  clutter_x11_texture_pixmap_update_area (CLUTTER_X11_TEXTURE_PIXMAP (priv->texture),
					  x, y, width, height);
#endif

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
 * Creates a new #GtkClutterActor.
 *
 * This widget can be used to embed a #GtkWidget into a Clutter scene,
 * by retrieving the internal #GtkBin container using
 * gtk_clutter_actor_get_widget() and adding the #GtkWidget to it.
 *
 * Return value: the newly created #GtkClutterActor
 */
ClutterActor *
gtk_clutter_actor_new (void)
{
  return g_object_new (GTK_CLUTTER_TYPE_ACTOR, NULL);
}

/**
 * gtk_clutter_actor_new_with_contents:
 * @contents: a #GtkWidget to pack into this #ClutterActor
 *
 * Creates a new #GtkClutterActor widget. This widget can be
 * used to embed a Gtk widget into a clutter scene.
 *
 * This function is the logical equivalent of:
 *
 * |[
 * ClutterActor *actor = gtk_clutter_actor_new ();
 * GtkWidget *bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (actor));
 *
 * gtk_container_add (GTK_CONTAINER (bin), contents);
 * ]|
 *
 * Return value: the newly created #GtkClutterActor
 */
ClutterActor *
gtk_clutter_actor_new_with_contents (GtkWidget *contents)
{
  g_return_val_if_fail (GTK_IS_WIDGET (contents), NULL);

  return g_object_new (GTK_CLUTTER_TYPE_ACTOR,
                       "contents", contents,
                       NULL);
}

/**
 * gtk_clutter_actor_get_widget:
 * @actor: a #GtkClutterActor
 *
 * Retrieves the #GtkBin used to hold the #GtkClutterActor:contents widget
 *
 * Return value: (transfer none): a #GtkBin
 */
GtkWidget *
gtk_clutter_actor_get_widget (GtkClutterActor *actor)
{
  g_return_val_if_fail (GTK_CLUTTER_IS_ACTOR (actor), NULL);

  return actor->priv->widget;
}

/**
 * gtk_clutter_actor_get_contents:
 * @actor: a #GtkClutterActor
 *
 * Retrieves the child of the #GtkBin used to hold the contents of @actor.
 *
 * This convenience function is the logical equivalent of:
 *
 * |[
 * GtkWidget *bin;
 *
 * bin = gtk_clutter_actor_get_widget (GTK_CLUTTER_ACTOR (actor));
 * return gtk_bin_get_child (GTK_BIN (bin));
 * ]|
 *
 * Return value: (transfer none): a #GtkWidget, or %NULL if not content
 *   has been set
 */
GtkWidget *
gtk_clutter_actor_get_contents (GtkClutterActor *actor)
{
  g_return_val_if_fail (GTK_CLUTTER_IS_ACTOR (actor), NULL);

  return gtk_bin_get_child (GTK_BIN (actor->priv->widget));
}
