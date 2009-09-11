/* gtk-clutter-standin.h: a widget that stands-in for a ClutterActor
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
 *   Danielle Madeley  <danielle.madeley@collabora.co.uk>
 */

#if !defined(__CLUTTER_GTK_H_INSIDE__) && !defined(CLUTTER_GTK_COMPILATION)
#error "Only <clutter-gtk/clutter-gtk.h> can be included directly."
#endif

#ifndef __GTK_CLUTTER_STANDIN_H__
#define __GTK_CLUTTER_STANDIN_H__

#include <gtk/gtk.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GTK_CLUTTER_TYPE_STANDIN          (gtk_clutter_standin_get_type ())
#define GTK_CLUTTER_STANDIN(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), GTK_CLUTTER_TYPE_STANDIN, GtkClutterStandin))
#define GTK_CLUTTER_IS_STANDIN(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), GTK_CLUTTER_TYPE_STANDIN))
#define GTK_CLUTTER_STANDIN_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST ((k), GTK_CLUTTER_TYPE_STANDIN, GtkClutterStandinClass))
#define GTK_CLUTTER_IS_STANDIN_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), GTK_CLUTTER_TYPE_STANDIN))
#define GTK_CLUTTER_STANDIN_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), GTK_CLUTTER_TYPE_STANDIN, GtkClutterStandinClass))

typedef struct _GtkClutterStandin         GtkClutterStandin;
typedef struct _GtkClutterStandinPrivate  GtkClutterStandinPrivate;
typedef struct _GtkClutterStandinClass    GtkClutterStandinClass;

/**
 * GtkClutterStandin:
 *
 * A #GtkWidget containing the default Clutter stage.
 *
 * Since: 1.0
 */
struct _GtkClutterStandin
{
  /*< private >*/
  GtkWidget parent_instance;

  GtkClutterStandinPrivate *priv;
};

/**
 * GtkClutterStandinClass:
 *
 * Base class for #GtkClutterStandin.
 *
 * Since: 1.0
 */
struct _GtkClutterStandinClass
{
  /*< private >*/
  GtkWidgetClass parent_class;

  /* padding for future expansion */
  void (*_clutter_gtk_reserved1) (void);
  void (*_clutter_gtk_reserved2) (void);
  void (*_clutter_gtk_reserved3) (void);
  void (*_clutter_gtk_reserved4) (void);
  void (*_clutter_gtk_reserved5) (void);
  void (*_clutter_gtk_reserved6) (void);
};

GType         gtk_clutter_standin_get_type  (void) G_GNUC_CONST;
GtkWidget *   gtk_clutter_standin_new       (ClutterActor *actor);
void          gtk_clutter_standin_set_actor (GtkClutterStandin *standin,
                                             ClutterActor      *actor);

G_END_DECLS

#endif /* __GTK_CLUTTER_STANDIN_H__ */
