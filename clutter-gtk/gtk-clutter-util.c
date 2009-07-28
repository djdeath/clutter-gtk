#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gtk-clutter-util.h"

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <clutter/clutter.h>

#if defined(HAVE_CLUTTER_GTK_X11)

#include <clutter/x11/clutter-x11.h>
#include <gdk/gdkx.h>

#elif defined(HAVE_CLUTTER_GTK_WIN32)

#include <clutter/clutter-win32.h>
#include <gdk/gdkwin32.h>

#endif /* HAVE_CLUTTER_GTK_{X11,WIN32} */

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

enum
{
  /* base symbols from GtkRcStyle */
  FG_COMPONENT = GTK_RC_FG,
  BG_COMPONENT = GTK_RC_BG,
  TEXT_COMPONENT = GTK_RC_TEXT,
  BASE_COMPONENT = GTK_RC_BASE,

  /* symbols used by GtkStyle */
  LIGHT_COMPONENT,
  MID_COMPONENT,
  DARK_COMPONENT,
  TEXT_AA_COMPONENT
};

static inline void
gtk_clutter_get_component (GtkWidget    *widget,
                           gint          component,
                           GtkStateType  state,
                           ClutterColor *color)
{
  GtkStyle *style = gtk_widget_get_style (widget);
  GdkColor gtk_color = { 0, };

  switch (component)
    {
    case FG_COMPONENT:
      gtk_color = style->fg[state];
      break;

    case BG_COMPONENT:
      gtk_color = style->bg[state];
      break;

    case TEXT_COMPONENT:
      gtk_color = style->text[state];
      break;

    case BASE_COMPONENT:
      gtk_color = style->base[state];
      break;

    case LIGHT_COMPONENT:
      gtk_color = style->light[state];
      break;

    case MID_COMPONENT:
      gtk_color = style->mid[state];
      break;

    case DARK_COMPONENT:
      gtk_color = style->dark[state];
      break;

    case TEXT_AA_COMPONENT:
      gtk_color = style->text_aa[state];
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

  gtk_clutter_get_component (widget, FG_COMPONENT, state, color);
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

  gtk_clutter_get_component (widget, BG_COMPONENT, state, color);
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

  gtk_clutter_get_component (widget, TEXT_COMPONENT, state, color);
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

  gtk_clutter_get_component (widget, BASE_COMPONENT, state, color);
}

/**
 * gtk_clutter_get_light_color:
 * @widget: a #GtkWidget
 * @state: a state
 * @color: return location for a #ClutterColor
 *
 * Retrieves the light color of @widget for the given @state and copies it
 * into @color.
 *
 * Since: 0.8
 */
void
gtk_clutter_get_light_color (GtkWidget    *widget,
                             GtkStateType  state,
                             ClutterColor *color)
{
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (state >= GTK_STATE_NORMAL &&
                    state <= GTK_STATE_INSENSITIVE);
  g_return_if_fail (color != NULL);

  gtk_clutter_get_component (widget, LIGHT_COMPONENT, state, color);
}

/**
 * gtk_clutter_get_mid_color:
 * @widget: a #GtkWidget
 * @state: a state
 * @color: return location for a #ClutterColor
 *
 * Retrieves the mid color of @widget for the given @state and copies it
 * into @color.
 *
 * Since: 0.8
 */
void
gtk_clutter_get_mid_color (GtkWidget    *widget,
                           GtkStateType  state,
                           ClutterColor *color)
{
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (state >= GTK_STATE_NORMAL &&
                    state <= GTK_STATE_INSENSITIVE);
  g_return_if_fail (color != NULL);

  gtk_clutter_get_component (widget, MID_COMPONENT, state, color);
}

/**
 * gtk_clutter_get_dark_color:
 * @widget: a #GtkWidget
 * @state: a state
 * @color: return location for a #ClutterColor
 *
 * Retrieves the dark color of @widget for the given @state and copies it
 * into @color.
 *
 * Since: 0.8
 */
void
gtk_clutter_get_dark_color (GtkWidget    *widget,
                            GtkStateType  state,
                            ClutterColor *color)
{
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (state >= GTK_STATE_NORMAL &&
                    state <= GTK_STATE_INSENSITIVE);
  g_return_if_fail (color != NULL);

  gtk_clutter_get_component (widget, DARK_COMPONENT, state, color);
}

/**
 * gtk_clutter_get_text_aa_color:
 * @widget: a #GtkWidget
 * @state: a state
 * @color: return location for a #ClutterColor
 *
 * Retrieves the text-aa color of @widget for the given @state and copies it
 * into @color.
 *
 * Since: 0.8
 */
void
gtk_clutter_get_text_aa_color (GtkWidget    *widget,
                               GtkStateType  state,
                               ClutterColor *color)
{
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (state >= GTK_STATE_NORMAL &&
                    state <= GTK_STATE_INSENSITIVE);
  g_return_if_fail (color != NULL);

  gtk_clutter_get_component (widget, TEXT_AA_COMPONENT, state, color);
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

GQuark
gtk_clutter_texture_error_quark (void)
{
  return g_quark_from_static_string ("clutter-gtk-texture-error-quark");
}

/**
 * gtk_clutter_texture_set_from_pixbuf:
 * @texture: a #ClutterTexture
 * @pixbuf: a #GdkPixbuf
 * @error: a return location for errors
 *
 * Sets the contents of @texture with a copy of @pixbuf.
 *
 * Return value: %TRUE on success, %FALSE on failure.
 *
 * Since: 0.8
 */
gboolean
gtk_clutter_texture_set_from_pixbuf (ClutterTexture *texture,
                                     GdkPixbuf      *pixbuf,
                                     GError        **error)
{
  g_return_val_if_fail (CLUTTER_IS_TEXTURE (texture), FALSE);
  g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), FALSE);

  return clutter_texture_set_from_rgb_data (texture,
                                     gdk_pixbuf_get_pixels (pixbuf),
                                     gdk_pixbuf_get_has_alpha (pixbuf),
                                     gdk_pixbuf_get_width (pixbuf),
                                     gdk_pixbuf_get_height (pixbuf),
                                     gdk_pixbuf_get_rowstride (pixbuf),
                                     gdk_pixbuf_get_has_alpha (pixbuf) ? 4 : 3,
                                     0,
                                     error);
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
 * @error: a return location for errors
 *
 * Sets the contents of @texture using the stock icon @stock_id, as
 * rendered by @widget.
 *
 * Return value: %TRUE on success, %FALSE on failure.
 *
 * Since: 0.8
 */
gboolean
gtk_clutter_texture_set_from_stock (ClutterTexture *texture,
                                    GtkWidget      *widget,
                                    const gchar    *stock_id,
                                    GtkIconSize     size,
                                    GError        **error)
{
  GdkPixbuf *pixbuf;
  gboolean returnval;

  g_return_val_if_fail (CLUTTER_IS_TEXTURE (texture), FALSE);
  g_return_val_if_fail (GTK_IS_WIDGET (widget), FALSE);
  g_return_val_if_fail (stock_id != NULL, FALSE);
  g_return_val_if_fail ((size > GTK_ICON_SIZE_INVALID) || (size == -1), FALSE);

  pixbuf = gtk_widget_render_icon (widget, stock_id, size, NULL);
  if (!pixbuf)
    {
      g_set_error (error,
                   CLUTTER_GTK_TEXTURE_ERROR,
                   CLUTTER_GTK_TEXTURE_INVALID_STOCK_ID,
                   "Stock ID '%s' not found", stock_id);
      return FALSE;
    }

  returnval = gtk_clutter_texture_set_from_pixbuf (texture, pixbuf, error);
  g_object_unref (pixbuf);

  return returnval;
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
 * @error: a return location for errors
 *
 * Sets the contents of @texture using the @icon_name from the
 * current icon theme.
 *
 * Return value: %TRUE on success, %FALSE on failure.
 *
 * Since: 0.8
 */
gboolean
gtk_clutter_texture_set_from_icon_name (ClutterTexture *texture,
                                        GtkWidget      *widget,
                                        const gchar    *icon_name,
                                        GtkIconSize     size,
                                        GError        **error)
{
  GError *local_error = NULL;
  GtkSettings *settings;
  GtkIconTheme *icon_theme;
  gboolean returnval;
  gint width, height;
  GdkPixbuf *pixbuf;

  g_return_val_if_fail (CLUTTER_IS_TEXTURE (texture), FALSE);
  g_return_val_if_fail (GTK_IS_WIDGET (widget), FALSE);
  g_return_val_if_fail (icon_name != NULL, FALSE);
  g_return_val_if_fail ((size > GTK_ICON_SIZE_INVALID) || (size == -1), FALSE);

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

  pixbuf = gtk_icon_theme_load_icon (icon_theme,
                                     icon_name,
                                     MIN (width, height), 0,
                                     &local_error);
  if (local_error)
    {
      g_propagate_error (error, local_error);
      return FALSE;
    }

  returnval = gtk_clutter_texture_set_from_pixbuf (texture, pixbuf, error);
  g_object_unref (pixbuf);

  return returnval;
}

/**
 * gtk_clutter_init:
 * @argc: pointer to the arguments count, or %NULL
 * @argv: pointer to the arguments vector, or %NULL
 *
 * This function should be called instead of clutter_init() and
 * gtk_init().
 *
 * Return value: %CLUTTER_INIT_SUCCESS on success, a negative integer
 *   on failure.
 *
 * Since: 0.8
 */
ClutterInitError
gtk_clutter_init (int    *argc,
                  char ***argv)
{
  if (!gtk_init_check (argc, argv))
    return CLUTTER_INIT_ERROR_GTK;

#if defined(HAVE_CLUTTER_GTK_X11)
  clutter_x11_set_display (GDK_DISPLAY());
  clutter_x11_disable_event_retrieval ();
#elif defined(HAVE_CLUTTER_GTK_WIN32)
  clutter_win32_disable_event_retrieval ();
#endif /* HAVE_CLUTTER_GTK_{X11,WIN32} */

  return clutter_init (argc, argv);
}

/**
 * gtk_clutter_init_with_args:
 * @argc: a pointer to the number of command line arguments.
 * @argv: a pointer to the array of command line arguments.
 * @parameter_string: a string which is displayed in
 *    the first line of <option>--help</option> output, after
 *    <literal><replaceable>programname</replaceable> [OPTION...]</literal>
 * @entries: a %NULL-terminated array of #GOptionEntry<!-- -->s
 *    describing the options of your program
 * @translation_domain: a translation domain to use for translating
 *    the <option>--help</option> output for the options in @entries
 *    with gettext(), or %NULL
 * @error: a return location for errors
 *
 * This function should be called instead of clutter_init() and
 * gtk_init_with_args().
 *
 * Return value: %CLUTTER_INIT_SUCCESS on success, a negative integer
 *   on failure.
 *
 * Since: 0.10
 */
ClutterInitError
gtk_clutter_init_with_args (int            *argc,
                            char         ***argv,
                            const char     *parameter_string,
                            GOptionEntry   *entries,
                            const char     *translation_domain,
                            GError        **error)
{
  gboolean res;

  res = gtk_init_with_args (argc, argv,
                            (char*) parameter_string,
                            entries,
                            (char*) translation_domain,
                            error);

  if (!res)
    return CLUTTER_INIT_ERROR_GTK;

#if defined(GDK_WINDOWING_X11)
  clutter_x11_set_display (GDK_DISPLAY());
  clutter_x11_disable_event_retrieval ();
#elif defined(GDK_WINDOWING_WIN32)
  clutter_win32_disable_event_retrieval ();
#endif /* GDK_WINDOWING_{X11,WIN32} */

  return clutter_init_with_args (argc, argv,
                                 NULL,
                                 NULL,
                                 NULL,
                                 error);
}
