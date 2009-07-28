/**
 * SECTION:gtk-clutter-viewport
 * @short_description: A scrollable actor
 *
 * #GtkClutterViewport is a scrollable actor that can contain a single
 * #ClutterActor. Using two #GtkAdjustment<!-- -->s it is possible to
 * control the visible area of the child actor if the size of the viewport
 * is smaller than the size of the child.
 *
 * The #GtkAdjustment<!-- -->s used to control the horizontal and
 * vertical scrolling can be attached to a #GtkScrollbar subclass,
 * like #GtkHScrollbar or #GtkVScrollbar.
 *
 * The #GtkClutterViewport can be used inside any #ClutterContainer
 * implementation.
 *
 * #GtkClutterViewport is available since Clutter-GTK 0.10
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cogl/cogl.h>

#include "gtk-clutter-viewport.h"

#include "gtk-clutter-scrollable.h"
#include "gtk-clutter-util.h"
#include "gtk-clutter-zoomable.h"

/* XXX - GtkAdjustment accessors have been added with GTK+ 2.14,
 * but I want Clutter-GTK to be future-proof, so let's do this
 * little #define dance.
 */
#if !GTK_CHECK_VERSION (2, 14, 0)
#define gtk_adjustment_set_page_size(a,v)       ((a)->page_size = (v))
#define gtk_adjustment_set_upper(a,v)           ((a)->upper = (v))
#define gtk_adjustment_set_page_increment(a,v)  ((a)->page_increment = (v))
#define gtk_adjustment_set_step_increment(a,v)  ((a)->step_increment = (v))
#define gtk_adjustment_set_lower(a,v)           ((a)->lower = (v))

#define gtk_adjustment_get_lower(a)             ((a)->lower)
#define gtk_adjustment_get_upper(a)             ((a)->upper)
#define gtk_adjustment_get_page_size(a)         ((a)->page_size)
#endif

#define GET_PRIVATE(obj)        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_CLUTTER_TYPE_VIEWPORT, GtkClutterViewportPrivate))

#define I_(str) (g_intern_static_string ((str)))

typedef enum {
  VIEWPORT_X_AXIS,
  VIEWPORT_Y_AXIS,
  VIEWPORT_Z_AXIS
} ViewportAxis;

static void clutter_container_iface_init      (gpointer g_iface);
static void gtk_clutter_scrollable_iface_init (gpointer g_iface);
static void gtk_clutter_zoomable_iface_init   (gpointer g_iface);

G_DEFINE_TYPE_WITH_CODE (GtkClutterViewport,
                         gtk_clutter_viewport,
                         CLUTTER_TYPE_ACTOR,
                         G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_CONTAINER,
                                                clutter_container_iface_init)
                         G_IMPLEMENT_INTERFACE (GTK_CLUTTER_TYPE_SCROLLABLE,
                                                gtk_clutter_scrollable_iface_init)
                         G_IMPLEMENT_INTERFACE (GTK_CLUTTER_TYPE_ZOOMABLE,
                                                gtk_clutter_zoomable_iface_init));

struct _GtkClutterViewportPrivate
{
  ClutterVertex origin;

  ClutterActor *child;

  GtkAdjustment *x_adjustment;
  GtkAdjustment *y_adjustment;
  GtkAdjustment *z_adjustment;
};

enum
{
  PROP_0,

  PROP_CHILD,
  PROP_ORIGIN,

  PROP_H_ADJUSTMENT,
  PROP_V_ADJUSTMENT,
  PROP_Z_ADJUSTMENT
};

static void
gtk_clutter_viewport_add (ClutterContainer *container,
                          ClutterActor     *actor)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (container)->priv;

  if (priv->child)
    clutter_actor_unparent (priv->child);

  clutter_actor_set_parent (actor, CLUTTER_ACTOR (container));
  priv->child = actor;

  clutter_actor_queue_relayout (CLUTTER_ACTOR (container));

  g_signal_emit_by_name (container, "actor-added", actor);
  g_object_notify (G_OBJECT (container), "child");
}

static void
gtk_clutter_viewport_remove (ClutterContainer *container,
                             ClutterActor     *actor)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (container)->priv;

  if (G_LIKELY (priv->child == actor))
    {
      g_object_ref (actor);

      clutter_actor_unparent (actor);
      priv->child = NULL;

      clutter_actor_queue_relayout (CLUTTER_ACTOR (container));

      g_signal_emit_by_name (container, "actor-removed", actor);

      g_object_unref (actor);
    }
}

static void
gtk_clutter_viewport_foreach (ClutterContainer *container,
                              ClutterCallback   callback,
                              gpointer          callback_data)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (container)->priv;

  if (G_LIKELY (priv->child))
    callback (priv->child, callback_data);
}

static void
clutter_container_iface_init (gpointer g_iface)
{
  ClutterContainerIface *iface = g_iface;

  iface->add = gtk_clutter_viewport_add;
  iface->remove = gtk_clutter_viewport_remove;
  iface->foreach = gtk_clutter_viewport_foreach;
}

static void
viewport_adjustment_value_changed (GtkAdjustment      *adjustment,
                                   GtkClutterViewport *viewport)
{
  GtkClutterViewportPrivate *priv = viewport->priv;

  if (priv->child && CLUTTER_ACTOR_IS_VISIBLE (priv->child))
    {
      GtkAdjustment *x_adjust = priv->x_adjustment;
      GtkAdjustment *y_adjust = priv->y_adjustment;
      GtkAdjustment *z_adjust = priv->z_adjustment;
      gfloat new_x, new_y, new_z;

      new_x = gtk_adjustment_get_value (x_adjust);
      new_y = gtk_adjustment_get_value (y_adjust);
      new_z = gtk_adjustment_get_value (z_adjust);

      /* change the origin and queue a relayout */
      if (new_x != priv->origin.x ||
          new_y != priv->origin.y ||
          new_z != priv->origin.z)
        {
          priv->origin.x = new_x;
          priv->origin.y = new_y;
          priv->origin.z = new_z;

          clutter_actor_queue_relayout (CLUTTER_ACTOR (viewport));

          g_object_notify (G_OBJECT (viewport), "origin");
        }
    }
}

static gboolean
viewport_reclamp_adjustment (GtkAdjustment *adjustment)
{
  gdouble value = gtk_adjustment_get_value (adjustment);
  gdouble limit;

  limit = gtk_adjustment_get_upper (adjustment)
        - gtk_adjustment_get_page_size (adjustment);

  value = CLAMP (value, 0, limit);
  if (value != gtk_adjustment_get_value (adjustment))
    {
      gtk_adjustment_set_value (adjustment, value);
      return TRUE;
    }
  else
    return FALSE;
}

static gboolean
viewport_set_hadjustment_values (GtkClutterViewport *viewport,
                                 gfloat              width)
{
  GtkClutterViewportPrivate *priv = viewport->priv;
  GtkAdjustment *h_adjust = priv->x_adjustment;

  if (width < 0)
    clutter_actor_get_preferred_width (CLUTTER_ACTOR (viewport), -1,
                                       NULL,
                                       &width);

  gtk_adjustment_set_page_size (h_adjust, width);
  gtk_adjustment_set_step_increment (h_adjust, width * 0.1);
  gtk_adjustment_set_page_increment (h_adjust, width * 0.9);
  gtk_adjustment_set_lower (h_adjust, 0);

  if (priv->child && CLUTTER_ACTOR_IS_VISIBLE (priv->child))
    {
      gfloat natural_width;

      clutter_actor_get_preferred_size (priv->child,
                                        NULL, NULL,
                                        &natural_width, NULL);

      gtk_adjustment_set_upper (h_adjust, MAX (natural_width, width));
    }
  else
    gtk_adjustment_set_upper (h_adjust, width);

  return viewport_reclamp_adjustment (h_adjust);
}

static gboolean
viewport_set_vadjustment_values (GtkClutterViewport *viewport,
                                 gfloat              height)
{
  GtkClutterViewportPrivate *priv = viewport->priv;
  GtkAdjustment *v_adjust = priv->y_adjustment;

  if (height < 0)
    clutter_actor_get_preferred_height (CLUTTER_ACTOR (viewport), -1,
                                        NULL,
                                        &height);

  gtk_adjustment_set_page_size (v_adjust, height);
  gtk_adjustment_set_step_increment (v_adjust, height * 0.1);
  gtk_adjustment_set_page_increment (v_adjust, height * 0.9);
  gtk_adjustment_set_lower (v_adjust, 0);

  if (priv->child && CLUTTER_ACTOR_IS_VISIBLE (priv->child))
    {
      gfloat natural_height;

      clutter_actor_get_preferred_size (priv->child,
                                        NULL, NULL,
                                        NULL, &natural_height);

      gtk_adjustment_set_upper (v_adjust, MAX (natural_height, height));
    }
  else
    gtk_adjustment_set_upper (v_adjust, height);

  return viewport_reclamp_adjustment (v_adjust);
}

static gboolean
viewport_set_zadjustment_values (GtkClutterViewport *viewport,
                                 gfloat              width,
                                 gfloat              height)
{
  GtkClutterViewportPrivate *priv = viewport->priv;
  GtkAdjustment *z_adjust = priv->z_adjustment;
  gfloat depth;

  if (width < 0)
    clutter_actor_get_preferred_width (CLUTTER_ACTOR (viewport), -1,
                                       NULL,
                                       &width);

  if (height < 0)
    clutter_actor_get_preferred_height (CLUTTER_ACTOR (viewport), -1,
                                        NULL,
                                        &height);

  depth = clutter_actor_get_depth (CLUTTER_ACTOR (viewport));

  gtk_adjustment_set_page_size (z_adjust, 0.0);
  gtk_adjustment_set_step_increment (z_adjust, MIN (width, height) * 0.1);
  gtk_adjustment_set_page_increment (z_adjust, MIN (width, height) * 0.9);
  gtk_adjustment_set_lower (z_adjust, 0.0);
  gtk_adjustment_set_upper (z_adjust, MAX (width, height));

#if 0
  g_debug ("%s: zadjustment: { %.2f lower, %.2f page, %.2f upper }",
           G_STRLOC,
           gtk_adjustment_get_lower (z_adjust),
           gtk_adjustment_get_page_size (z_adjust),
           gtk_adjustment_get_upper (z_adjust));
#endif

  return viewport_reclamp_adjustment (z_adjust);
}

static inline void
disconnect_adjustment (GtkClutterViewport *viewport,
                       ViewportAxis        axis)
{
  GtkClutterViewportPrivate *priv = viewport->priv;
  GtkAdjustment **adj_p = NULL;

  switch (axis)
    {
    case VIEWPORT_X_AXIS:
      adj_p = &priv->x_adjustment;
      break;

    case VIEWPORT_Y_AXIS:
      adj_p = &priv->y_adjustment;
      break;

    case VIEWPORT_Z_AXIS:
      adj_p = &priv->z_adjustment;
      break;

    default:
      g_assert_not_reached ();
      break;
    }

  if (*adj_p != NULL)
    {
      g_signal_handlers_disconnect_by_func (*adj_p,
                                            viewport_adjustment_value_changed,
                                            viewport);
      g_object_unref (*adj_p);
      *adj_p = NULL;
    }
}

static inline void
connect_adjustment (GtkClutterViewport *viewport,
                    ViewportAxis        axis,
                    GtkAdjustment      *adjustment)
{
  GtkClutterViewportPrivate *priv = viewport->priv;
  GtkAdjustment **adj_p = NULL;
  gboolean value_changed = FALSE;
  gfloat width, height;

  switch (axis)
    {
    case VIEWPORT_X_AXIS:
      adj_p = &priv->x_adjustment;
      break;

    case VIEWPORT_Y_AXIS:
      adj_p = &priv->y_adjustment;
      break;

    case VIEWPORT_Z_AXIS:
      adj_p = &priv->z_adjustment;
      break;

    default:
      g_assert_not_reached ();
      break;
    }

  if (adjustment && adjustment == *adj_p)
    return;

  if (!adjustment)
    adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 0, 0, 0, 0));

  disconnect_adjustment (viewport, axis);
  *adj_p = g_object_ref_sink (adjustment);

  clutter_actor_get_size (CLUTTER_ACTOR (viewport), &width, &height);

  switch (axis)
    {
    case VIEWPORT_X_AXIS:
      value_changed = viewport_set_hadjustment_values (viewport, width);
      break;

    case VIEWPORT_Y_AXIS:
      value_changed = viewport_set_vadjustment_values (viewport, height);
      break;

    case VIEWPORT_Z_AXIS:
      value_changed = viewport_set_zadjustment_values (viewport,
                                                       width,
                                                       height);
      break;

    default:
      g_assert_not_reached ();
      break;
    }

  g_signal_connect (adjustment, "value-changed",
                    G_CALLBACK (viewport_adjustment_value_changed),
                    viewport);

  gtk_adjustment_changed (adjustment);

  if (value_changed)
    gtk_adjustment_value_changed (adjustment);
  else
    viewport_adjustment_value_changed (adjustment, viewport);

  switch (axis)
    {
    case VIEWPORT_X_AXIS:
      g_object_notify (G_OBJECT (viewport), "hadjustment");
      break;

    case VIEWPORT_Y_AXIS:
      g_object_notify (G_OBJECT (viewport), "vadjustment");
      break;

    case VIEWPORT_Z_AXIS:
      g_object_notify (G_OBJECT (viewport), "zadjustment");
      break;

    default:
      g_assert_not_reached ();
      break;
    }
}

static void
scrollable_set_adjustments (GtkClutterScrollable *scrollable,
                            GtkAdjustment        *h_adjust,
                            GtkAdjustment        *v_adjust)
{
  g_object_freeze_notify (G_OBJECT (scrollable));

  connect_adjustment (GTK_CLUTTER_VIEWPORT (scrollable),
                      VIEWPORT_X_AXIS,
                      h_adjust);
  connect_adjustment (GTK_CLUTTER_VIEWPORT (scrollable),
                      VIEWPORT_Y_AXIS,
                      v_adjust);

  g_object_thaw_notify (G_OBJECT (scrollable));
}

static void
scrollable_get_adjustments (GtkClutterScrollable  *scrollable,
                            GtkAdjustment        **h_adjust,
                            GtkAdjustment        **v_adjust)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (scrollable)->priv;

  if (h_adjust)
    *h_adjust = priv->x_adjustment;

  if (v_adjust)
    *v_adjust = priv->y_adjustment;
}

static void
gtk_clutter_scrollable_iface_init (gpointer g_iface)
{
  GtkClutterScrollableIface *iface = g_iface;

  iface->set_adjustments = scrollable_set_adjustments;
  iface->get_adjustments = scrollable_get_adjustments;
}

static void
zoomable_set_adjustment (GtkClutterZoomable *zoomable,
                         GtkAdjustment      *adjust)
{
  g_object_freeze_notify (G_OBJECT (zoomable));

  connect_adjustment (GTK_CLUTTER_VIEWPORT (zoomable),
                      VIEWPORT_Z_AXIS,
                      adjust);

  g_object_thaw_notify (G_OBJECT (zoomable));
}

static GtkAdjustment *
zoomable_get_adjustment (GtkClutterZoomable *zoomable)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (zoomable)->priv;

  return priv->z_adjustment;
}

static void
gtk_clutter_zoomable_iface_init (gpointer g_iface)
{
  GtkClutterZoomableIface *iface = g_iface;

  iface->set_adjustment = zoomable_set_adjustment;
  iface->get_adjustment = zoomable_get_adjustment;
}

static void
gtk_clutter_viewport_set_property (GObject      *gobject,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (gobject)->priv;

  switch (prop_id)
    {
    case PROP_CHILD:
      clutter_container_add_actor (CLUTTER_CONTAINER (gobject),
                                   g_value_get_object (value));
      break;

    case PROP_ORIGIN:
      {
        ClutterVertex *v = g_value_get_boxed (value);

        priv->origin = *v;

        if (CLUTTER_ACTOR_IS_VISIBLE (gobject))
          clutter_actor_queue_redraw (CLUTTER_ACTOR (gobject));
      }
      break;

    case PROP_H_ADJUSTMENT:
      connect_adjustment (GTK_CLUTTER_VIEWPORT (gobject),
                          VIEWPORT_X_AXIS,
                          g_value_get_object (value));
      break;

    case PROP_V_ADJUSTMENT:
      connect_adjustment (GTK_CLUTTER_VIEWPORT (gobject),
                          VIEWPORT_Y_AXIS,
                          g_value_get_object (value));
      break;

    case PROP_Z_ADJUSTMENT:
      connect_adjustment (GTK_CLUTTER_VIEWPORT (gobject),
                          VIEWPORT_Z_AXIS,
                          g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
gtk_clutter_viewport_get_property (GObject    *gobject,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (gobject)->priv;

  switch (prop_id)
    {
    case PROP_CHILD:
      g_value_set_object (value, priv->child);
      break;

    case PROP_ORIGIN:
      g_value_set_boxed (value, &priv->origin);
      break;

    case PROP_H_ADJUSTMENT:
      g_value_set_object (value, priv->x_adjustment);
      break;

    case PROP_V_ADJUSTMENT:
      g_value_set_object (value, priv->y_adjustment);
      break;

    case PROP_Z_ADJUSTMENT:
      g_value_set_object (value, priv->z_adjustment);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
gtk_clutter_viewport_dispose (GObject *gobject)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (gobject)->priv;

  if (priv->child)
    {
      clutter_actor_destroy (priv->child);
      priv->child = NULL;
    }

  disconnect_adjustment (GTK_CLUTTER_VIEWPORT (gobject),
                         VIEWPORT_X_AXIS);
  disconnect_adjustment (GTK_CLUTTER_VIEWPORT (gobject),
                         VIEWPORT_Y_AXIS);
  disconnect_adjustment (GTK_CLUTTER_VIEWPORT (gobject),
                         VIEWPORT_Z_AXIS);

  G_OBJECT_CLASS (gtk_clutter_viewport_parent_class)->dispose (gobject);
}

static void
gtk_clutter_viewport_get_preferred_width (ClutterActor *actor,
                                          gfloat        for_height,
                                          gfloat       *min_width_p,
                                          gfloat       *natural_width_p)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (actor)->priv;

  /* we don't have a minimum size */
  if (min_width_p)
    *min_width_p = 0;

  /* if we have a child, we want to be as big as the child
   * wishes to be; otherwise, we don't have a preferred width
   */
  if (priv->child)
    clutter_actor_get_preferred_width (priv->child, for_height,
                                       NULL,
                                       natural_width_p);
  else
    {
      if (natural_width_p)
        *natural_width_p = 0;
    }
}

static void
gtk_clutter_viewport_get_preferred_height (ClutterActor *actor,
                                           gfloat        for_width,
                                           gfloat       *min_height_p,
                                           gfloat       *natural_height_p)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (actor)->priv;

  /* we don't have a minimum size */
  if (min_height_p)
    *min_height_p = 0;

  /* if we have a child, we want to be as big as the child
   * wishes to be; otherwise, we don't have a preferred height
   */
  if (priv->child)
    clutter_actor_get_preferred_height (priv->child, for_width,
                                        NULL,
                                        natural_height_p);
  else
    {
      if (natural_height_p)
        *natural_height_p = 0;
    }
}

static void
gtk_clutter_viewport_allocate (ClutterActor           *actor,
                               const ClutterActorBox  *box,
                               ClutterAllocationFlags  flags)
{
  GtkClutterViewport *viewport = GTK_CLUTTER_VIEWPORT (actor);
  GtkClutterViewportPrivate *priv = viewport->priv;
  ClutterActorClass *parent_class;
  gboolean x_adjustment_value_changed;
  gboolean y_adjustment_value_changed;
  gboolean z_adjustment_value_changed;
  gfloat width, height;

  parent_class = CLUTTER_ACTOR_CLASS (gtk_clutter_viewport_parent_class);
  parent_class->allocate (actor, box, flags);

  width  = box->x2 - box->x1;
  height = box->y2 - box->y1;

  x_adjustment_value_changed =
    viewport_set_hadjustment_values (viewport, width);
  y_adjustment_value_changed =
    viewport_set_vadjustment_values (viewport, height);
  z_adjustment_value_changed =
    viewport_set_zadjustment_values (viewport, width, height);

  if (priv->child && CLUTTER_ACTOR_IS_VISIBLE (priv->child))
    {
      ClutterActorBox child_allocation = { 0, };
      gfloat alloc_width, alloc_height;

      /* a viewport is a boundless actor which can contain a child
       * without constraints; hence, we give any child exactly the
       * wanted natural size, no matter how small the viewport
       * actually is.
       */
      clutter_actor_get_preferred_size (priv->child,
                                        NULL, NULL,
                                        &alloc_width,
                                        &alloc_height);

      child_allocation.x1 = 0;
      child_allocation.y1 = 0;
      child_allocation.x2 = child_allocation.x1 + alloc_width;
      child_allocation.y2 = child_allocation.y1 + alloc_height;

      clutter_actor_allocate (priv->child, &child_allocation, flags);
    }

  gtk_adjustment_changed (priv->x_adjustment);
  gtk_adjustment_changed (priv->y_adjustment);
  gtk_adjustment_changed (priv->z_adjustment);

  if (x_adjustment_value_changed)
    gtk_adjustment_value_changed (priv->x_adjustment);

  if (y_adjustment_value_changed)
    gtk_adjustment_value_changed (priv->y_adjustment);

  if (z_adjustment_value_changed)
    gtk_adjustment_value_changed (priv->z_adjustment);
}

static void
gtk_clutter_viewport_apply_transform (ClutterActor *actor,
                                      CoglMatrix   *matrix)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (actor)->priv;
  ClutterActorClass *parent_class;

  parent_class = CLUTTER_ACTOR_CLASS (gtk_clutter_viewport_parent_class);
  parent_class->apply_transform (actor, matrix);

  cogl_matrix_translate (matrix,
                         priv->origin.x * -1,
                         priv->origin.y * -1,
                         priv->origin.z * -1);
}

static void
gtk_clutter_viewport_paint (ClutterActor *actor)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (actor)->priv;

  if (priv->child)
    clutter_actor_paint (priv->child);
}

static void
gtk_clutter_viewport_pick (ClutterActor       *actor,
                           const ClutterColor *pick_color)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (actor)->priv;
  ClutterActorClass *parent_class;

  parent_class = CLUTTER_ACTOR_CLASS (gtk_clutter_viewport_parent_class);
  parent_class->pick (actor, pick_color);

  if (priv->child)
    clutter_actor_paint (priv->child);
}

static void
gtk_clutter_viewport_class_init (GtkClutterViewportClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);
  GParamSpec *pspec;

  g_type_class_add_private (klass, sizeof (GtkClutterViewportPrivate));

  gobject_class->set_property = gtk_clutter_viewport_set_property;
  gobject_class->get_property = gtk_clutter_viewport_get_property;
  gobject_class->dispose = gtk_clutter_viewport_dispose;

  actor_class->get_preferred_width = gtk_clutter_viewport_get_preferred_width;
  actor_class->get_preferred_height = gtk_clutter_viewport_get_preferred_height;
  actor_class->allocate = gtk_clutter_viewport_allocate;
  actor_class->apply_transform = gtk_clutter_viewport_apply_transform;
  actor_class->paint = gtk_clutter_viewport_paint;
  actor_class->pick = gtk_clutter_viewport_pick;

  /**
   * GtkClutterViewport:child:
   *
   * The #ClutterActor inside the viewport.
   *
   * Since: 0.10
   */
  pspec = g_param_spec_object ("child",
                               "Child",
                               "The ClutterActor inside the viewport",
                               CLUTTER_TYPE_ACTOR,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_CHILD, pspec);

  /**
   * GtkClutterViewport:origin:
   *
   * The current origin of the viewport. You should use the
   * vertex to convert event coordinates for the child of the
   * viewport.
   *
   * Since: 0.10
   */
  pspec = g_param_spec_boxed ("origin",
                              "Origin",
                              "The current origin of the viewport",
                              CLUTTER_TYPE_VERTEX,
                              G_PARAM_READABLE);
  g_object_class_install_property (gobject_class, PROP_ORIGIN, pspec);

  /* GtkClutterScrollable properties */
  g_object_class_override_property (gobject_class,
                                    PROP_H_ADJUSTMENT,
                                    "hadjustment");
  g_object_class_override_property (gobject_class,
                                    PROP_V_ADJUSTMENT,
                                    "vadjustment");

  /* GtkClutterZoomable properties */
  g_object_class_override_property (gobject_class,
                                    PROP_Z_ADJUSTMENT,
                                    "zadjustment");
}

static void
gtk_clutter_viewport_init (GtkClutterViewport *viewport)
{
  GtkClutterViewportPrivate *priv;

  viewport->priv = priv = GET_PRIVATE (viewport);
}

/**
 * gtk_clutter_viewport_new:
 * @h_adjust: horizontal adjustment, or %NULL
 * @v_adjust: vertical adjustment, or %NULL
 * @z_adjust: zoom adjustment, or %NULL
 *
 * Creates a new #GtkClutterViewport with the given adjustments.
 *
 * Return value: the newly created viewport actor
 *
 * Since: 0.10
 */
ClutterActor *
gtk_clutter_viewport_new (GtkAdjustment *h_adjust,
                          GtkAdjustment *v_adjust,
                          GtkAdjustment *z_adjust)
{
  return g_object_new (GTK_CLUTTER_TYPE_VIEWPORT,
                       "hadjustment", h_adjust,
                       "vadjustment", v_adjust,
                       "zadjustment", z_adjust,
                       NULL);
}

/**
 * gtk_clutter_viewport_get_origin:
 * @viewport: a #GtkClutterViewport
 * @x: return location for the X origin in pixels, or %NULL
 * @y: return location for the Y origin in pixels, or %NULL
 * @z: return location for the Z origin in pixels, or %NULL
 *
 * Retrieves the current translation factor ("origin") used when
 * displaying the child of @viewport.
 *
 * Since: 0.10
 */
void
gtk_clutter_viewport_get_origin (GtkClutterViewport *viewport,
                                 gfloat             *x,
                                 gfloat             *y,
                                 gfloat             *z)
{
  GtkClutterViewportPrivate *priv;

  g_return_if_fail (GTK_CLUTTER_IS_VIEWPORT (viewport));

  priv = viewport->priv;

  if (x)
    *x = priv->origin.x;

  if (y)
    *y = priv->origin.y;

  if (z)
    *z = priv->origin.z;
}
