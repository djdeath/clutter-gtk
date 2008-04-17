#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include <clutter/clutter.h>

#include "gtk-clutter-util.h"

/**
 * SECTION:gtk-clutter-util
 * @short_description: Utility functions for integrating Clutter in GTK+
 *
 * FIXME
 *
 */

static inline void
gtk_clutter_get_component (GtkWidget    *widget,
                           GtkRcFlags    component,
                           GtkStateType  state,
                           ClutterColor *color)
{
  GtkStyle *style = gtk_widget_get_style (widget);
  GdkColor gtk_color = { 0, };

  switch (component)
    {
    case GTK_RC_FG:
      gtk_color = style->fg[state];
      break;

    case GTK_RC_BG:
      gtk_color = style->bg[state];
      break;

    case GTK_RC_TEXT:
      gtk_color = style->text[state];
      break;

    case GTK_RC_BASE:
      gtk_color = style->base[state];
      break;

    default:
      g_assert_not_reached ();
      break;
    }

  color->red   = (guint8) ((gtk_color.red   / 65535.0) * 255);
  color->green = (guint8) ((gtk_color.green / 65535.0) * 255);
  color->blue  = (guint8) ((gtk_color.blue  / 65535.0) * 255);
}

/**
 * gtk_clutter_get_fg_color:
 * @widget:
 * @state:
 * @color:
 *
 * FIXME
 *
 * Since: 0.8
 */
void
gtk_clutter_get_fg_color (GtkWidget    *widget,
                          GtkStateType  state,
                          ClutterColor *color)
{
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (state >= GTK_STATE_NORMAL &&
                    state <= GTK_STATE_INSENSITIVE);
  g_return_if_fail (color != NULL);

  gtk_clutter_get_component (widget, GTK_RC_FG, state, color);
}

/**
 * gtk_clutter_get_bg_color:
 * @widget:
 * @state:
 * @color:
 *
 * FIXME
 *
 * Since: 0.8
 */
void
gtk_clutter_get_bg_color (GtkWidget    *widget,
                          GtkStateType  state,
                          ClutterColor *color)
{
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (state >= GTK_STATE_NORMAL &&
                    state <= GTK_STATE_INSENSITIVE);
  g_return_if_fail (color != NULL);

  gtk_clutter_get_component (widget, GTK_RC_BG, state, color);
}

/**
 * gtk_clutter_get_text_color:
 * @widget:
 * @state:
 * @color:
 *
 * FIXME
 *
 * Since: 0.8
 */
void
gtk_clutter_get_text_color (GtkWidget    *widget,
                            GtkStateType  state,
                            ClutterColor *color)
{
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (state >= GTK_STATE_NORMAL &&
                    state <= GTK_STATE_INSENSITIVE);
  g_return_if_fail (color != NULL);

  gtk_clutter_get_component (widget, GTK_RC_TEXT, state, color);
}

/**
 * gtk_clutter_get_base_color:
 * @widget:
 * @state:
 * @color:
 *
 * FIXME
 *
 * Since: 0.8
 */
void
gtk_clutter_get_base_color (GtkWidget    *widget,
                            GtkStateType  state,
                            ClutterColor *color)
{
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (state >= GTK_STATE_NORMAL &&
                    state <= GTK_STATE_INSENSITIVE);
  g_return_if_fail (color != NULL);

  gtk_clutter_get_component (widget, GTK_RC_BASE, state, color);
}

