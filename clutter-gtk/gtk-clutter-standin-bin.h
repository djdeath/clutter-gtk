/* gtk-clutter-standin-bin.h: a ClutterContainer used by GtkClutterStandin
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

#ifndef __GTK_CLUTTER_STANDIN_BIN_H__
#define __GTK_CLUTTER_STANDIN_BIN_H__

#include <gtk/gtk.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GTK_CLUTTER_TYPE_STANDIN_BIN          (gtk_clutter_standin_bin_get_type ())
#define GTK_CLUTTER_STANDIN_BIN(o)            (G_TYPE_CHECK_INSTANCE_CAST ((o), GTK_CLUTTER_TYPE_STANDIN_BIN, GtkClutterStandinBin))
#define GTK_CLUTTER_IS_STANDIN_BIN(o)         (G_TYPE_CHECK_INSTANCE_TYPE ((o), GTK_CLUTTER_TYPE_STANDIN_BIN))
#define GTK_CLUTTER_STANDIN_BIN_CLASS(k)      (G_TYPE_CHECK_CLASS_CAST ((k), GTK_CLUTTER_TYPE_STANDIN_BIN, GtkClutterStandinBinClass))
#define GTK_CLUTTER_IS_STANDIN_BIN_CLASS(k)   (G_TYPE_CHECK_CLASS_TYPE ((k), GTK_CLUTTER_TYPE_STANDIN_BIN))
#define GTK_CLUTTER_STANDIN_BIN_GET_CLASS(o)  (G_TYPE_INSTANCE_GET_CLASS ((o), GTK_CLUTTER_TYPE_STANDIN_BIN, GtkClutterStandinBinClass))

typedef struct _GtkClutterStandinBin         GtkClutterStandinBin;
typedef struct _GtkClutterStandinBinClass    GtkClutterStandinBinClass;

struct _GtkClutterStandinBin
{
  /*< private >*/
  ClutterGroup parent_instance;

  GtkWidget    *standin;
  ClutterActor *child;
};

struct _GtkClutterStandinBinClass
{
  /*< private >*/
  ClutterGroupClass parent_class;
};

GType gtk_clutter_standin_bin_get_type            (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __GTK_CLUTTER_STANDIN_BIN_H__ */
