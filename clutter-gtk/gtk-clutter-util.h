/* gtk-clutter-util.h: GTK+ integration utilities
 *
 * Copyright (C) 2008 OpenedHand
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
 *   Emmanuele Bassi  <ebassi@openedhand.com>
 */

#ifndef __GTK_CLUTTER_UTIL_H__
#define __GTK_CLUTTER_UTIL_H__

#include <gtk/gtk.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

void          gtk_clutter_get_fg_color               (GtkWidget      *widget,
                                                      GtkStateType    state,
                                                      ClutterColor   *color);
void          gtk_clutter_get_bg_color               (GtkWidget      *widget,
                                                      GtkStateType    state,
                                                      ClutterColor   *color);
void          gtk_clutter_get_text_color             (GtkWidget      *widget,
                                                      GtkStateType    state,
                                                      ClutterColor   *color);
void          gtk_clutter_get_text_aa_color          (GtkWidget      *widget,
                                                      GtkStateType    state,
                                                      ClutterColor   *color);
void          gtk_clutter_get_base_color             (GtkWidget      *widget,
                                                      GtkStateType    state,
                                                      ClutterColor   *color);
void          gtk_clutter_get_light_color            (GtkWidget      *widget,
                                                      GtkStateType    state,
                                                      ClutterColor   *color);
void          gtk_clutter_get_dark_color             (GtkWidget      *widget,
                                                      GtkStateType    state,
                                                      ClutterColor   *color);
void          gtk_clutter_get_mid_color              (GtkWidget      *widget,
                                                      GtkStateType    state,
                                                      ClutterColor   *color);

ClutterActor *gtk_clutter_texture_new_from_pixbuf    (GdkPixbuf      *pixbuf);
ClutterActor *gtk_clutter_texture_new_from_stock     (GtkWidget      *widget,
                                                      const gchar    *stock_id,
                                                      GtkIconSize     size);
ClutterActor *gtk_clutter_texture_new_from_icon_name (GtkWidget      *widget,
                                                      const gchar    *icon_name,
                                                      GtkIconSize     size);
void          gtk_clutter_texture_set_from_pixbuf    (ClutterTexture *texture,
                                                      GdkPixbuf      *pixbuf);
void          gtk_clutter_texture_set_from_stock     (ClutterTexture *texture,
                                                      GtkWidget      *widget,
                                                      const gchar    *stock_id,
                                                      GtkIconSize     size);
void          gtk_clutter_texture_set_from_icon_name (ClutterTexture *texture,
                                                      GtkWidget      *widget,
                                                      const gchar    *icon_name,
                                                      GtkIconSize     size);

G_END_DECLS

#endif /* __GTK_CLUTTER_UTIL_H__ */
