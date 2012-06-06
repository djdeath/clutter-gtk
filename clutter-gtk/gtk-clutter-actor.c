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

#ifdef CLUTTER_WINDOWING_X11
#include <clutter/x11/clutter-x11.h>
#endif
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#endif
#ifdef CAIRO_HAS_XLIB_SURFACE
#include <cairo/cairo-xlib.h>
#endif

#ifdef CLUTTER_WINDOWING_WIN32
#include <clutter/clutter-win32.h>
#endif
#ifdef GDK_WINDOWING_WIN32
#include <gdk/gdkwin32.h>
#endif

G_DEFINE_TYPE (GtkClutterActor, gtk_clutter_actor, CLUTTER_TYPE_ACTOR)

#define GTK_CLUTTER_ACTOR_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_CLUTTER_TYPE_ACTOR, GtkClutterActorPrivate))

struct _GtkClutterActorPrivate
{
  GtkWidget *widget;
  GtkWidget *embed;

  cairo_surface_t *surface;

  ClutterActor *texture;
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

  stage = clutter_actor_get_stage (actor);
  priv->embed = g_object_get_data (G_OBJECT (stage), "gtk-clutter-embed");
  gtk_container_add (GTK_CONTAINER (priv->embed), priv->widget);

  gtk_widget_realize (priv->widget);

  priv->surface = _gtk_clutter_offscreen_get_surface (GTK_CLUTTER_OFFSCREEN (priv->widget));

#if defined(CLUTTER_WINDOWING_X11) && defined(CAIRO_HAS_XLIB_SURFACE)
  if (clutter_check_windowing_backend (CLUTTER_WINDOWING_X11) &&
      cairo_surface_get_type (priv->surface) == CAIRO_SURFACE_TYPE_XLIB)
    {
      Drawable pixmap;
      gint pixmap_width, pixmap_height;

      pixmap_width = cairo_xlib_surface_get_width (priv->surface);
      pixmap_height = cairo_xlib_surface_get_height (priv->surface);
      pixmap = cairo_xlib_surface_get_drawable (priv->surface);

      clutter_x11_texture_pixmap_set_pixmap (CLUTTER_X11_TEXTURE_PIXMAP (priv->texture), pixmap);
      clutter_actor_set_size (priv->texture, pixmap_width, pixmap_height);
    }
#endif
}

static void
gtk_clutter_actor_unrealize (ClutterActor *actor)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  GtkClutterActorPrivate *priv = clutter->priv;

  if (priv->widget == NULL)
    return;

  priv->surface = NULL;

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
  gint min_width, natural_width;

  min_width = natural_width = 0;

  if (for_height >= 0)
    {
      for_height = ceilf (for_height);
      gtk_widget_get_preferred_width_for_height (priv->widget,
                                                 for_height,
                                                 &min_width,
                                                 &natural_width);
    }
  else
    {
      gtk_widget_get_preferred_width (priv->widget,
                                      &min_width,
                                      &natural_width);
    }

  if (min_width_p)
    *min_width_p = min_width;

  if (natural_width_p)
    *natural_width_p = natural_width;
}

static void
gtk_clutter_actor_get_preferred_height (ClutterActor *actor,
                                        gfloat        for_width,
                                        gfloat       *min_height_p,
                                        gfloat       *natural_height_p)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  GtkClutterActorPrivate *priv = clutter->priv;
  gint min_height, natural_height;

  min_height = natural_height = 0;

  if (for_width >= 0)
    {
      for_width = ceilf (for_width);
      gtk_widget_get_preferred_height_for_width (priv->widget,
                                                 for_width,
                                                 &min_height,
                                                 &natural_height);
    }
  else
    {
      gtk_widget_get_preferred_height (priv->widget,
                                       &min_height,
                                       &natural_height);
    }

  if (min_height_p)
    *min_height_p = min_height;

  if (natural_height_p)
    *natural_height_p = natural_height;
}

static void
gtk_clutter_actor_allocate (ClutterActor           *actor,
                            const ClutterActorBox  *box,
                            ClutterAllocationFlags  flags)
{
  GtkClutterActor *clutter = GTK_CLUTTER_ACTOR (actor);
  GtkClutterActorPrivate *priv = clutter->priv;
  GtkAllocation child_allocation;
  GdkWindow *window;
  ClutterActorBox child_box;

  _gtk_clutter_offscreen_set_in_allocation (GTK_CLUTTER_OFFSCREEN (priv->widget), TRUE);

  child_allocation.x = 0;
  child_allocation.y = 0;
  child_allocation.width = clutter_actor_box_get_width (box);
  child_allocation.height = clutter_actor_box_get_height (box);
  gtk_widget_size_allocate (priv->widget, &child_allocation);

  if (CLUTTER_ACTOR_IS_REALIZED (actor))
    {
      cairo_surface_t *surface;

      /* The former size allocate may have queued an expose we then need to
       * process immediately, since we will paint the pixmap when this
       * returns (as size allocation is done from clutter_redraw which is
       * called from gtk_clutter_embed_expose_event(). If we don't do this
       * we may see an intermediate state of the pixmap, causing flicker
       */
      window = gtk_widget_get_window (priv->widget);
      gdk_window_process_updates (window, TRUE);

      surface = gdk_offscreen_window_get_surface (window);
      if (surface != priv->surface)
        {
#if defined(CLUTTER_WINDOWING_X11) && defined(CAIRO_HAS_XLIB_SURFACE)
          if (clutter_check_windowing_backend (CLUTTER_WINDOWING_X11) &&
              cairo_surface_get_type (surface) == CAIRO_SURFACE_TYPE_XLIB)
            {
              Drawable pixmap = cairo_xlib_surface_get_drawable (surface);

              clutter_x11_texture_pixmap_set_pixmap (CLUTTER_X11_TEXTURE_PIXMAP (priv->texture), pixmap);
            }
#endif

          priv->surface = surface;
        }
    }

  _gtk_clutter_offscreen_set_in_allocation (GTK_CLUTTER_OFFSCREEN (priv->widget), FALSE);

  clutter_actor_set_allocation (actor, box, (flags | CLUTTER_DELEGATE_LAYOUT));

  child_box.x1 = child_box.y1 = 0.f;
  child_box.x2 = clutter_actor_box_get_width (box);
  child_box.y2 = clutter_actor_box_get_height (box);

  /* we force the allocation of the offscreen texture */
  clutter_actor_allocate (priv->texture, &child_box, flags);
}

static void
gtk_clutter_actor_paint (ClutterActor *actor)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (actor)->priv;
  ClutterActorIter iter;
  ClutterActor *child;

  /* we always paint the texture below everything else */
  clutter_actor_paint (priv->texture);

  clutter_actor_iter_init (&iter, actor);
  while (clutter_actor_iter_next (&iter, &child))
    clutter_actor_paint (child);
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
}

static void
gtk_clutter_actor_hide (ClutterActor *self)
{
  GtkClutterActorPrivate *priv = GTK_CLUTTER_ACTOR (self)->priv;
  GtkWidget *widget;

  CLUTTER_ACTOR_CLASS (gtk_clutter_actor_parent_class)->hide (self);

  /* proxy this call through to GTK+ */
  widget = gtk_bin_get_child (GTK_BIN (priv->widget));
  if (widget != NULL)
    gtk_widget_hide (widget);
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

  actor_class->paint = gtk_clutter_actor_paint;
  actor_class->realize = gtk_clutter_actor_realize;
  actor_class->unrealize = gtk_clutter_actor_unrealize;
  actor_class->show = gtk_clutter_actor_show;
  actor_class->hide = gtk_clutter_actor_hide;

  actor_class->get_preferred_width = gtk_clutter_actor_get_preferred_width;
  actor_class->get_preferred_height = gtk_clutter_actor_get_preferred_height;
  actor_class->allocate = gtk_clutter_actor_allocate;

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
                               G_PARAM_READWRITE |
                               G_PARAM_CONSTRUCT |
                               G_PARAM_STATIC_STRINGS);
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

  clutter_actor_set_reactive (actor, TRUE);

#if defined(CLUTTER_WINDOWING_X11)
  if (clutter_check_windowing_backend (CLUTTER_WINDOWING_X11))
    {
      priv->texture = clutter_x11_texture_pixmap_new ();

      clutter_texture_set_sync_size (CLUTTER_TEXTURE (priv->texture), FALSE);
      clutter_actor_add_child (actor, priv->texture);
      clutter_actor_set_name (priv->texture, "Onscreen Texture");
      clutter_actor_show (priv->texture);
    }
  else
#endif
    g_critical ("Embedding GtkWidget inside ClutterActor through "
                "GtkClutterActor does not yet work on non-X11 "
                "platforms.");

  g_signal_connect (self, "notify::reactive", G_CALLBACK (on_reactive_change), NULL);
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
#if defined(CLUTTER_WINDOWING_X11)
  if (clutter_check_windowing_backend (CLUTTER_WINDOWING_X11))
    {
      GtkClutterActorPrivate *priv = actor->priv;

      clutter_x11_texture_pixmap_update_area (CLUTTER_X11_TEXTURE_PIXMAP (priv->texture),
					      x, y, width, height);
    }
#endif

  clutter_actor_queue_redraw (CLUTTER_ACTOR (actor));
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
