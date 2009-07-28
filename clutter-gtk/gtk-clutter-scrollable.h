/* gtk-clutter-scrollable.h: Interface for scrollable actors
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
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author:
 *   Emmanuele Bassi <ebassi@linux.intel.com>
 *
 * Based on similar code by:
 *   Chris Lord <chris@openedhand.com>
 */

#if !defined(__CLUTTER_GTK_H_INSIDE__) && !defined(CLUTTER_GTK_COMPILATION)
#error "Only <clutter-gtk/clutter-gtk.h> can be included directly."
#endif

#ifndef __GTK_CLUTTER_SCROLLABLE_H__
#define __GTK_CLUTTER_SCROLLABLE_H__

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTK_CLUTTER_TYPE_SCROLLABLE             (gtk_clutter_scrollable_get_type ())
#define GTK_CLUTTER_SCROLLABLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_CLUTTER_TYPE_SCROLLABLE, GtkClutterScrollable))
#define GTK_CLUTTER_IS_SCROLLABLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_CLUTTER_TYPE_SCROLLABLE))
#define GTK_CLUTTER_SCROLLABLE_GET_IFACE(obj)   (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GTK_CLUTTER_TYPE_SCROLLABLE, GtkClutterScrollableIface))

typedef struct _GtkClutterScrollable            GtkClutterScrollable; /* dummy */
typedef struct _GtkClutterScrollableIface       GtkClutterScrollableIface;

/**
 * GtkClutterScrollableIface:
 * @set_adjustments: virtual function for setting the horizontal
 *   and vertical adjustments of a #GtkClutterScrollable
 * @get_adjustments: virtual function for retrieving the horizontal
 *   and vertical adjustments of a #GtkClutterScrollable
 *
 * The #GtkClutterScrollableIface structure contains only private data
 * and should be accessed using the provided functions.
 *
 * Since: 0.10
 */
struct _GtkClutterScrollableIface
{
  /*< private >*/
  GTypeInterface parent_iface;

  /*< public >*/
  void (* set_adjustments) (GtkClutterScrollable  *scrollable,
                            GtkAdjustment         *h_adjust,
                            GtkAdjustment         *v_adjust);
  void (* get_adjustments) (GtkClutterScrollable  *scrollable,
                            GtkAdjustment        **h_adjust,
                            GtkAdjustment        **v_adjust);
};

GType gtk_clutter_scrollable_get_type (void) G_GNUC_CONST;

void gtk_clutter_scrollable_set_adjustments (GtkClutterScrollable  *scrollable,
                                             GtkAdjustment         *h_adjust,
                                             GtkAdjustment         *v_adjust);
void gtk_clutter_scrollable_get_adjustments (GtkClutterScrollable  *scrollable,
                                             GtkAdjustment        **h_adjust,
                                             GtkAdjustment        **v_adjust);

G_END_DECLS

#endif /* __GTK_CLUTTER_SCROLLABLE_H__ */
