#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gtk-clutter-scrollable.h"

#define I_(str) (g_intern_static_string ((str)))

/**
 * SECTION:gtk-clutter-scrollable
 * @short_description: Interface for scrollable actors
 *
 * #GtkClutterScrollable is an interface for scrollable actors, reusing
 * the #GtkAdjustment objects from GTK+ do drive the scrolling.
 *
 * #GtkClutterScrollable is available since Clutter-GTK 0.10
 */

static void
gtk_clutter_scrollable_base_init (gpointer g_iface)
{
  static gboolean is_initialized = FALSE;

  if (G_UNLIKELY (!is_initialized))
    {
      GParamSpec *pspec;

      /**
       * GtkClutterScrollable:hadjustment:
       *
       * The #GtkAdjustment that determines the value of the
       * horizontal position for this scrollable actor.
       *
       * Since: 0.10
       */
      pspec = g_param_spec_object ("hadjustment",
                                   "Horizontal adjustment",
                                   "The GtkAdjustment that determines "
                                   "the value of the horizontal position "
                                   "for this scrollable actor",
                                   GTK_TYPE_ADJUSTMENT,
                                   G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
      g_object_interface_install_property (g_iface, pspec);

      /**
       * GtkClutterScrollable:vadjustment:
       *
       * The #GtkAdjustment that determines the value of the
       * vertical position for this scrollable actor.
       *
       * Since: 0.10
       */
      pspec = g_param_spec_object ("vadjustment",
                                   "Vertical adjustment",
                                   "The GtkAdjustment that determines "
                                   "the value of the vertical position "
                                   "for this scrollable actor",
                                   GTK_TYPE_ADJUSTMENT,
                                   G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
      g_object_interface_install_property (g_iface, pspec);

      is_initialized = TRUE;
    }
}

GType
gtk_clutter_scrollable_get_type (void)
{
  static GType scrollable_type = 0;

  if (G_UNLIKELY (!scrollable_type))
    {
      const GTypeInfo scrollable_info =
      {
        sizeof (GtkClutterScrollableIface),
        (GBaseInitFunc)     gtk_clutter_scrollable_base_init,
        (GBaseFinalizeFunc) NULL,
      };

      scrollable_type = g_type_register_static (G_TYPE_INTERFACE,
                                                I_("GtkClutterScrollable"),
                                                &scrollable_info, 0);
    }

  return scrollable_type;
}

/**
 * gtk_clutter_scrollable_set_adjustments:
 * @scrollable: a #GtkClutterScrollable
 * @h_adjust: a #GtkAdjustment, or %NULL
 * @v_adjust: a #GtkAdjustment, or %NULL
 *
 * Sets the horizontal and vertical adjustments used to determine
 * the position of the scrollable actor.
 *
 * Since: 0.10
 */
void
gtk_clutter_scrollable_set_adjustments (GtkClutterScrollable *scrollable,
                                        GtkAdjustment        *h_adjust,
                                        GtkAdjustment        *v_adjust)
{
  GtkClutterScrollableIface *iface;

  g_return_if_fail (GTK_CLUTTER_IS_SCROLLABLE (scrollable));
  g_return_if_fail (h_adjust == NULL || GTK_IS_ADJUSTMENT (h_adjust));

  iface = GTK_CLUTTER_SCROLLABLE_GET_IFACE (scrollable);
  if (iface->set_adjustments)
    iface->set_adjustments (scrollable, h_adjust, v_adjust);
}

/**
 * gtk_clutter_scrollable_get_adjustments:
 * @scrollable: a #GtkClutterScrollable
 * @h_adjust: return location for a #GtkAdjustment, or %NULL
 * @v_adjust: return location for a #GtkAdjustment, or %NULL
 *
 * Retrieves the horizontal and vertical adjustments used to
 * determine the position of the scrollable actor.
 *
 * Since: 0.10
 */
void
gtk_clutter_scrollable_get_adjustments (GtkClutterScrollable  *scrollable,
                                        GtkAdjustment        **h_adjust,
                                        GtkAdjustment        **v_adjust)
{
  GtkClutterScrollableIface *iface;

  g_return_if_fail (GTK_CLUTTER_IS_SCROLLABLE (scrollable));

  iface = GTK_CLUTTER_SCROLLABLE_GET_IFACE (scrollable);
  if (iface->get_adjustments)
    iface->get_adjustments (scrollable, h_adjust, v_adjust);
}
