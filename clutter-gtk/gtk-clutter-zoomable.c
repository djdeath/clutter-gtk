#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "gtk-clutter-zoomable.h"

#define I_(str) (g_intern_static_string ((str)))

/**
 * SECTION:gtk-clutter-zoomable
 * @short_description: Interface for zoomable actors
 *
 * #GtkClutterZoomable is an interface for zoomable actors, using, like
 * #GtkClutterScrollable, the #GtkAdjustment objects from GTK+ to drive
 * the zoom factor.
 *
 * #GtkClutterZoomable is available since Clutter-GTK 1.0
 */

static void
gtk_clutter_zoomable_base_init (gpointer g_iface)
{
  static gboolean is_initialized = FALSE;

  if (G_UNLIKELY (!is_initialized))
    {
      GParamSpec *pspec;

      /**
       * GtkClutterZoomable:zadjustment:
       *
       * The #GtkAdjustment that determines the value of
       * the zoom factor for this zoomable actor
       *
       * Since: 0.10
       */
      pspec = g_param_spec_object ("zadjustment",
                                   "Zoom Adjustment",
                                   "The GtkAdjustment that determines "
                                   "the zoom factor",
                                   GTK_TYPE_ADJUSTMENT,
                                   G_PARAM_READWRITE |
                                   G_PARAM_CONSTRUCT);
      g_object_interface_install_property (g_iface, pspec);

      is_initialized = TRUE;
    }
}

GType
gtk_clutter_zoomable_get_type (void)
{
  static GType zoomable_type = 0;

  if (G_UNLIKELY (!zoomable_type))
    {
      const GTypeInfo zoomable_info =
      {
        sizeof (GtkClutterZoomableIface),
        (GBaseInitFunc)     gtk_clutter_zoomable_base_init,
        (GBaseFinalizeFunc) NULL,
      };

      zoomable_type = g_type_register_static (G_TYPE_INTERFACE,
                                              I_("GtkClutterZoomable"),
                                              &zoomable_info, 0);
    }

  return zoomable_type;
}

/**
 * gtk_clutter_zoomable_set_adjustment:
 * @zoomable: a #GtkClutterZoomable
 * @z_adjust: (null-ok): a #GtkAdjustment, or %NULL
 *
 * Sets the adjustment used to determine the zoom factor of
 * the zoomable actor
 *
 * Since: 0.10
 */
void
gtk_clutter_zoomable_set_adjustment (GtkClutterZoomable *zoomable,
                                     GtkAdjustment      *z_adjust)
{
  GtkClutterZoomableIface *iface;

  g_return_if_fail (GTK_CLUTTER_IS_ZOOMABLE (zoomable));
  g_return_if_fail (z_adjust == NULL || GTK_IS_ADJUSTMENT (z_adjust));

  iface = GTK_CLUTTER_ZOOMABLE_GET_IFACE (zoomable);
  if (iface->set_adjustment)
    iface->set_adjustment (zoomable, z_adjust);
}

/**
 * gtk_clutter_zoomable_get_adjustment:
 * @zoomable: a #GtkClutterZoomable
 *
 * Retrieves the adjustment used to determine the zoom factor of
 * the zoomable actor
 *
 * Return value: (transfer none): a #GtkAdjustment
 *
 * Since: 0.10
 */
GtkAdjustment *
gtk_clutter_zoomable_get_adjustment (GtkClutterZoomable *zoomable)
{
  GtkClutterZoomableIface *iface;

  g_return_val_if_fail (GTK_CLUTTER_IS_ZOOMABLE (zoomable), NULL);

  iface = GTK_CLUTTER_ZOOMABLE_GET_IFACE (zoomable);
  if (iface->get_adjustment)
    return iface->get_adjustment (zoomable);

  return NULL;
}
