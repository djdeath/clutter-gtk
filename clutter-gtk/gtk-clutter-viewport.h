/* gtk-clutter-viewport.h: Scrollable actor
 *
 * Copyright (C) 2008  Intel Corp.
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
 *   Emmanuele Bassi  <ebassi@linux.intel.com>
 */

#if !defined(__CLUTTER_GTK_H_INSIDE__) && !defined(CLUTTER_GTK_COMPILATION)
#error "Only <clutter-gtk/clutter-gtk.h> can be included directly."
#endif

#ifndef __GTK_CLUTTER_VIEWPORT_H__
#define __GTK_CLUTTER_VIEWPORT_H__

#include <gtk/gtk.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GTK_CLUTTER_TYPE_VIEWPORT               (gtk_clutter_viewport_get_type ())
#define GTK_CLUTTER_VIEWPORT(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_CLUTTER_TYPE_VIEWPORT, GtkClutterViewport))
#define GTK_CLUTTER_IS_VIEWPORT(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_CLUTTER_TYPE_VIEWPORT))
#define GTK_CLUTTER_VIEWPORT_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_CLUTTER_TYPE_VIEWPORT, GtkClutterViewportClass))
#define GTK_CLUTTER_IS_VIEWPORT_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_CLUTTER_TYPE_VIEWPORT))
#define GTK_CLUTTER_VIEWPORT_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_CLUTTER_TYPE_VIEWPORT, GtkClutterViewportClass))

typedef struct _GtkClutterViewport              GtkClutterViewport;
typedef struct _GtkClutterViewportPrivate       GtkClutterViewportPrivate;
typedef struct _GtkClutterViewportClass         GtkClutterViewportClass;

/**
 * GtkClutterViewport:
 *
 * The #GtkClutterViewport structure contains only private data and
 * should be accessed using the provided functions.
 *
 * Since: 0.8.2
 */
struct _GtkClutterViewport
{
  /*< private >*/
  ClutterActor parent_instance;

  GtkClutterViewportPrivate *priv;
};

/**
 * GtkClutterViewportClass:
 *
 * The #GtkClutterViewportClass structure contains only private data and
 * should be accessed using the provided functions.
 *
 * Since: 0.8.2
 */
struct _GtkClutterViewportClass
{
  /*< private >*/
  ClutterActorClass parent_class;
};

GType gtk_clutter_viewport_get_type (void) G_GNUC_CONST;

ClutterActor *gtk_clutter_viewport_new         (GtkAdjustment      *h_adjust,
                                                GtkAdjustment      *v_adjust,
                                                GtkAdjustment      *z_adjust);

void          gtk_clutter_viewport_get_origin  (GtkClutterViewport *viewport,
                                                gfloat             *x,
                                                gfloat             *y,
                                                gfloat             *z);

G_END_DECLS

#endif /* __GTK_CLUTTER_VIEWPORT_H__ */
