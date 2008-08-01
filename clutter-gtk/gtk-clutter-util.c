#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <clutter/clutter.h>

#include "gtk-clutter-util.h"

/**
 * SECTION:gtk-clutter-util
 * @short_description: Utility functions for integrating Clutter in GTK+
 *
 * In order to properly integrate a Clutter scene into a GTK+ applications
 * a certain degree of state must be retrieved from GTK+ itself.
 *
 * Clutter-GTK provides API for easing the process of synchronizing colors
 * with the current GTK+ theme and for loading image sources from #GdkPixbuf,
 * GTK+ stock items and icon themes.
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

  color->red   = CLAMP (((gtk_color.red   / 65535.0) * 255), 0, 255);
  color->green = CLAMP (((gtk_color.green / 65535.0) * 255), 0, 255);
  color->blue  = CLAMP (((gtk_color.blue  / 65535.0) * 255), 0, 255);
  color->alpha = 255;
}

/**
 * gtk_clutter_get_fg_color:
 * @widget: a #GtkWidget
 * @state: a state
 * @color: return location for a #ClutterColor
 *
 * Retrieves the foreground color of @widget for the given @state and copies
 * it into @color.
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
 * @widget: a #GtkWidget
 * @state: a state
 * @color: return location for a #ClutterColor
 *
 * Retrieves the background color of @widget for the given @state and copies
 * it into @color.
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
 * @widget: a #GtkWidget
 * @state: a state
 * @color: return location for a #ClutterColor
 *
 * Retrieves the text color of @widget for the given @state and copies it
 * into @color.
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
 * @widget: a #GtkWidget
 * @state: a state
 * @color: return location for a #ClutterColor
 *
 * Retrieves the base color of @widget for the given @state and copies it
 * into @color.
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

/**
 * gtk_clutter_texture_new_from_pixbuf:
 * @pixbuf: a #GdkPixbuf
 *
 * Creates a new #ClutterTexture and sets its contents with a copy
 * of @pixbuf.
 *
 * Return value: the newly created #ClutterTexture
 *
 * Since: 0.8
 */
ClutterActor *
gtk_clutter_texture_new_from_pixbuf (GdkPixbuf *pixbuf)
{
  ClutterActor *retval;
  GError *error;

  g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), NULL);

  retval = clutter_texture_new ();

  error = NULL;
  clutter_texture_set_from_rgb_data (CLUTTER_TEXTURE (retval),
                                     gdk_pixbuf_get_pixels (pixbuf),
                                     gdk_pixbuf_get_has_alpha (pixbuf),
                                     gdk_pixbuf_get_width (pixbuf),
                                     gdk_pixbuf_get_height (pixbuf),
                                     gdk_pixbuf_get_rowstride (pixbuf),
                                     gdk_pixbuf_get_has_alpha (pixbuf) ? 4 : 3,
                                     0,
                                     &error);
  if (error)
    {
      g_warning ("Unable to set the pixbuf: %s", error->message);
      g_error_free (error);
    }

  return retval; 
}

/**
 * gtk_clutter_texture_set_from_pixbuf:
 * @texture: a #ClutterTexture
 * @pixbuf: a #GdkPixbuf
 *
 * Sets the contents of @texture with a copy of @pixbuf.
 *
 * Since: 0.8
 */
void
gtk_clutter_texture_set_from_pixbuf (ClutterTexture *texture,
                                     GdkPixbuf      *pixbuf)
{
  GError *error;

  g_return_if_fail (CLUTTER_IS_TEXTURE (texture));
  g_return_if_fail (GDK_IS_PIXBUF (pixbuf));

  error = NULL;
  clutter_texture_set_from_rgb_data (texture,
                                     gdk_pixbuf_get_pixels (pixbuf),
                                     gdk_pixbuf_get_has_alpha (pixbuf),
                                     gdk_pixbuf_get_width (pixbuf),
                                     gdk_pixbuf_get_height (pixbuf),
                                     gdk_pixbuf_get_rowstride (pixbuf),
                                     gdk_pixbuf_get_has_alpha (pixbuf) ? 4 : 3,
                                     0,
                                     &error);
  if (error)
    {
      g_warning ("Unable to set the pixbuf: %s", error->message);
      g_error_free (error);
    }
}

/**
 * gtk_clutter_texture_new_from_stock:
 * @widget: a #GtkWidget
 * @stock_id: the stock id of the icon
 * @size: the size of the icon, or -1
 *
 * Creates a new #ClutterTexture and sets its contents using the stock
 * icon @stock_id as rendered by @widget.
 *
 * Return value: the newly created #ClutterTexture
 *
 * Since: 0.8
 */
ClutterActor *
gtk_clutter_texture_new_from_stock (GtkWidget   *widget,
                                    const gchar *stock_id,
                                    GtkIconSize  size)
{
  GdkPixbuf *pixbuf;
  ClutterActor *retval;

  g_return_val_if_fail (GTK_IS_WIDGET (widget), NULL);
  g_return_val_if_fail (stock_id != NULL, NULL);
  g_return_val_if_fail (size > GTK_ICON_SIZE_INVALID || size == -1, NULL);

  pixbuf = gtk_widget_render_icon (widget, stock_id, size, NULL);
  if (!pixbuf)
    pixbuf = gtk_widget_render_icon (widget,
                                     GTK_STOCK_MISSING_IMAGE, size,
                                     NULL);

  retval = gtk_clutter_texture_new_from_pixbuf (pixbuf);
  g_object_unref (pixbuf);

  return retval;
}

/**
 * gtk_clutter_texture_set_from_stock:
 * @texture: a #ClutterTexture
 * @widget: a #GtkWidget
 * @stock_id: the stock id of the icon
 * @size: the size of the icon, or -1
 *
 * Sets the contents of @texture using the stock icon @stock_id, as
 * rendered by @widget.
 *
 * Since: 0.8
 */
void
gtk_clutter_texture_set_from_stock (ClutterTexture *texture,
                                    GtkWidget      *widget,
                                    const gchar    *stock_id,
                                    GtkIconSize     size)
{
  GdkPixbuf *pixbuf;

  g_return_if_fail (CLUTTER_IS_TEXTURE (texture));
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (stock_id != NULL);
  g_return_if_fail (size > GTK_ICON_SIZE_INVALID || size == -1);

  pixbuf = gtk_widget_render_icon (widget, stock_id, size, NULL);
  if (!pixbuf)
    pixbuf = gtk_widget_render_icon (widget,
                                     GTK_STOCK_MISSING_IMAGE, size,
                                     NULL);

  gtk_clutter_texture_set_from_pixbuf (texture, pixbuf);
  g_object_unref (pixbuf);
}

/**
 * gtk_clutter_texture_new_from_icon_name:
 * @widget: a #GtkWidget or %NULL
 * @icon_name: the name of the icon
 * @size: the size of the icon, or -1
 *
 * Creates a new #ClutterTexture and sets its contents to be
 * the @icon_name from the current icon theme.
 *
 * Return value: the newly created texture, or %NULL if @widget
 *   was %NULL and @icon_name was not found.
 *
 * Since: 0.8
 */
ClutterActor *
gtk_clutter_texture_new_from_icon_name (GtkWidget   *widget,
                                        const gchar *icon_name,
                                        GtkIconSize  size)
{
  GtkSettings *settings;
  GtkIconTheme *icon_theme;
  gint width, height;
  GdkPixbuf *pixbuf;
  GError *error;
  ClutterActor *retval;

  g_return_val_if_fail (widget == NULL || GTK_IS_WIDGET (widget), NULL);
  g_return_val_if_fail (icon_name != NULL, NULL);
  g_return_val_if_fail (size > GTK_ICON_SIZE_INVALID || size == -1, NULL);

  if (widget && gtk_widget_has_screen (widget))
    {
      GdkScreen *screen;

      screen = gtk_widget_get_screen (widget);
      settings = gtk_settings_get_for_screen (screen);
      icon_theme = gtk_icon_theme_get_for_screen (screen);
    }
  else
    {
      settings = gtk_settings_get_default ();
      icon_theme = gtk_icon_theme_get_default ();
    }

  if (size == -1 ||
      !gtk_icon_size_lookup_for_settings (settings, size, &width, &height))
    {
      width = height = 48;
    }

  error = NULL;
  pixbuf = gtk_icon_theme_load_icon (icon_theme,
                                     icon_name,
                                     MIN (width, height), 0,
                                     &error);
  if (error)
    {
      g_warning ("Unable to load the icon `%s' from the theme: %s",
                 icon_name,
                 error->message);

      g_error_free (error);

      if (widget)
        return gtk_clutter_texture_new_from_stock (widget,
                                             GTK_STOCK_MISSING_IMAGE,
                                             size);
      else
        return NULL;
    }

  retval = gtk_clutter_texture_new_from_pixbuf (pixbuf);
  g_object_unref (pixbuf);

  return retval; 
}

/**
 * gtk_clutter_texture_set_from_icon_name:
 * @texture: a #ClutterTexture
 * @widget: a #GtkWidget or %NULL
 * @icon_name: the name of the icon
 * @size: the icon size or -1
 *
 * Sets the contents of @texture using the @icon_name from the
 * current icon theme.
 *
 * Since: 0.8
 */
void
gtk_clutter_texture_set_from_icon_name (ClutterTexture *texture,
                                        GtkWidget      *widget,
                                        const gchar    *icon_name,
                                        GtkIconSize     size)
{
  GtkSettings *settings;
  GtkIconTheme *icon_theme;
  gint width, height;
  GdkPixbuf *pixbuf;
  GError *error;

  g_return_if_fail (CLUTTER_IS_TEXTURE (texture));
  g_return_if_fail (widget == NULL || GTK_IS_WIDGET (widget));
  g_return_if_fail (icon_name != NULL);
  g_return_if_fail (size > GTK_ICON_SIZE_INVALID || size == -1);

  if (widget && gtk_widget_has_screen (widget))
    {
      GdkScreen *screen;

      screen = gtk_widget_get_screen (widget);
      settings = gtk_settings_get_for_screen (screen);
      icon_theme = gtk_icon_theme_get_for_screen (screen);
    }
  else
    {
      settings = gtk_settings_get_default ();
      icon_theme = gtk_icon_theme_get_default ();
    }

  if (size == -1 ||
      !gtk_icon_size_lookup_for_settings (settings, size, &width, &height))
    {
      width = height = 48;
    }

  error = NULL;
  pixbuf = gtk_icon_theme_load_icon (icon_theme,
                                     icon_name,
                                     MIN (width, height), 0,
                                     &error);
  if (error)
    {
      g_warning ("Unable to load the icon `%s' from the theme: %s",
                 icon_name,
                 error->message);

      g_error_free (error);

      if (widget)
        gtk_clutter_texture_set_from_stock (texture,
                                      widget,
                                      GTK_STOCK_MISSING_IMAGE,
                                      size);
      else
        return;
    }

  gtk_clutter_texture_set_from_pixbuf (texture, pixbuf);
  g_object_unref (pixbuf);
}
