#ifndef __GTK_CLUTTER_UTIL_H__
#define __GTK_CLUTTER_UTIL_H__

#include <gtk/gtkwidget.h>
#include <clutter/clutter-color.h>

G_BEGIN_DECLS

void gtk_clutter_get_fg_color   (GtkWidget    *widget,
                                 GtkStateType  state,
                                 ClutterColor *color);
void gtk_clutter_get_bg_color   (GtkWidget    *widget,
                                 GtkStateType  state,
                                 ClutterColor *color);
void gtk_clutter_get_text_color (GtkWidget    *widget,
                                 GtkStateType  state,
                                 ClutterColor *color);
void gtk_clutter_get_base_color (GtkWidget    *widget,
                                 GtkStateType  state,
                                 ClutterColor *color);

G_END_DECLS

#endif /* __GTK_CLUTTER_UTIL_H__ */
