#if !defined(__CLUTTER_GTK_H_INSIDE__) && !defined(CLUTTER_GTK_COMPILATION)
#error "Only <clutter-gtk/clutter-gtk.h> can be included directly."
#endif

#ifndef __GTK_CLUTTER_ZOOMABLE_H__
#define __GTK_CLUTTER_ZOOMABLE_H__

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTK_CLUTTER_TYPE_ZOOMABLE               (gtk_clutter_zoomable_get_type ())
#define GTK_CLUTTER_ZOOMABLE(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_CLUTTER_TYPE_ZOOMABLE, GtkClutterZoomable))
#define GTK_CLUTTER_IS_ZOOMABLE(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_CLUTTER_TYPE_ZOOMABLE))
#define GTK_CLUTTER_ZOOMABLE_GET_IFACE(obj)     (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GTK_CLUTTER_TYPE_ZOOMABLE, GtkClutterZoomableIface))

typedef struct _GtkClutterZoomable      GtkClutterZoomable; /* dummy typedef */
typedef struct _GtkClutterZoomableIface GtkClutterZoomableIface;

/**
 * GtkClutterZoomableIface:
 * @set_adjustment: virtual function for setting the zoom adjustment
 * @get_adjustment: virtual function for getting the zoom adjustment
 *
 * The #GtkClutterZoomableIface structure contains only private data
 *
 * Since: 1.0
 */
struct _GtkClutterZoomableIface
{
  /*< private >*/
  GTypeInterface parent_iface;

  /*< public >*/
  void           (* set_adjustment) (GtkClutterZoomable *zoomable,
                                     GtkAdjustment      *z_adjust);

  GtkAdjustment *(* get_adjustment) (GtkClutterZoomable *zoomable);
};

GType gtk_clutter_zoomable_get_type (void) G_GNUC_CONST;

void           gtk_clutter_zoomable_set_adjustment (GtkClutterZoomable *zoomable,
                                                    GtkAdjustment      *z_adjust);
GtkAdjustment *gtk_clutter_zoomable_get_adjustment (GtkClutterZoomable *zoomable);

G_END_DECLS

#endif /* __GTK_CLUTTER_ZOOMABLE_H__ */
