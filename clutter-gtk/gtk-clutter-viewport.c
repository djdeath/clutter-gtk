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
 * #GtkClutterViewport is available since Clutter-GTK 1.0
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cogl/cogl.h>

#include "gtk-clutter-scrollable.h"
#include "gtk-clutter-util.h"
#include "gtk-clutter-viewport.h"

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

#define gtk_adjustment_get_upper(a)             ((a)->upper)
#define gtk_adjustment_get_page_size(a)         ((a)->page_size)
#endif

#define GET_PRIVATE(obj)        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_CLUTTER_TYPE_VIEWPORT, GtkClutterViewportPrivate))

#define I_(str) (g_intern_static_string ((str)))

static void clutter_container_iface_init      (gpointer g_iface);
static void gtk_clutter_scrollable_iface_init (gpointer g_iface);

G_DEFINE_TYPE_WITH_CODE (GtkClutterViewport,
                         gtk_clutter_viewport,
                         CLUTTER_TYPE_ACTOR,
                         G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_CONTAINER,
                                                clutter_container_iface_init)
                         G_IMPLEMENT_INTERFACE (GTK_CLUTTER_TYPE_SCROLLABLE,
                                                gtk_clutter_scrollable_iface_init));

struct _GtkClutterViewportPrivate
{
  ClutterVertex origin;

  ClutterActor *child;

  GtkAdjustment *h_adjustment;
  GtkAdjustment *v_adjustment;
};

enum
{
  PROP_0,

  PROP_CHILD,
  PROP_ORIGIN,
  PROP_H_ADJUSTMENT,
  PROP_V_ADJUSTMENT
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
      GtkAdjustment *h_adjust = priv->h_adjustment;
      GtkAdjustment *v_adjust = priv->v_adjustment;
      ClutterUnit new_x, new_y;

      new_x = CLUTTER_UNITS_FROM_FLOAT (gtk_adjustment_get_value (h_adjust));
      new_y = CLUTTER_UNITS_FROM_FLOAT (gtk_adjustment_get_value (v_adjust));

      /* change the origin and queue a relayout */
      if (new_x != priv->origin.x || new_y != priv->origin.y)
        {
          priv->origin.x = new_x;
          priv->origin.y = new_y;

          clutter_actor_queue_relayout (CLUTTER_ACTOR (viewport));
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
                                 guint               width)
{
  GtkClutterViewportPrivate *priv = viewport->priv;
  GtkAdjustment *h_adjust = priv->h_adjustment;

  gtk_adjustment_set_page_size (h_adjust, width);
  gtk_adjustment_set_step_increment (h_adjust, width * 0.1);
  gtk_adjustment_set_page_increment (h_adjust, width * 0.9);
  gtk_adjustment_set_lower (h_adjust, 0);

  if (priv->child && CLUTTER_ACTOR_IS_VISIBLE (priv->child))
    {
      ClutterUnit natural_width;

      clutter_actor_get_preferred_size (priv->child,
                                        NULL, NULL,
                                        &natural_width, NULL);

      gtk_adjustment_set_upper (h_adjust,
                                MAX (CLUTTER_UNITS_TO_DEVICE (natural_width),
                                     width));
    }
  else
    gtk_adjustment_set_upper (h_adjust, width);

  return viewport_reclamp_adjustment (h_adjust);
}

static gboolean
viewport_set_vadjustment_values (GtkClutterViewport *viewport,
                                 guint               height)
{
  GtkClutterViewportPrivate *priv = viewport->priv;
  GtkAdjustment *v_adjust = priv->v_adjustment;

  height = clutter_actor_get_height (CLUTTER_ACTOR (viewport));

  gtk_adjustment_set_page_size (v_adjust, height);
  gtk_adjustment_set_step_increment (v_adjust, height * 0.1);
  gtk_adjustment_set_page_increment (v_adjust, height * 0.9);
  gtk_adjustment_set_lower (v_adjust, 0);

  if (priv->child && CLUTTER_ACTOR_IS_VISIBLE (priv->child))
    {
      ClutterUnit natural_height;

      clutter_actor_get_preferred_size (priv->child,
                                        NULL, NULL,
                                        NULL, &natural_height);

      gtk_adjustment_set_upper (v_adjust,
                                MAX (CLUTTER_UNITS_TO_DEVICE (natural_height),
                                     height));
    }
  else
    gtk_adjustment_set_upper (v_adjust, height);

  return viewport_reclamp_adjustment (v_adjust);
}

static inline void
disconnect_adjustment (GtkClutterViewport *viewport,
                       GtkOrientation      orientation)
{
  GtkClutterViewportPrivate *priv = viewport->priv;
  GtkAdjustment **adj_p;

  adj_p = (orientation == GTK_ORIENTATION_HORIZONTAL) ? &priv->h_adjustment
                                                      : &priv->v_adjustment;

  if (*adj_p)
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
                    GtkOrientation      orientation,
                    GtkAdjustment      *adjustment)
{
  GtkClutterViewportPrivate *priv = viewport->priv;
  GtkAdjustment **adj_p;
  gboolean value_changed = FALSE;
  guint width, height;

  adj_p = (orientation == GTK_ORIENTATION_HORIZONTAL) ? &priv->h_adjustment
                                                      : &priv->v_adjustment;

  if (adjustment && adjustment == *adj_p)
    return;

  if (!adjustment)
    adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 0, 0, 0, 0));

  disconnect_adjustment (viewport, orientation);
  *adj_p = g_object_ref_sink (adjustment);

  clutter_actor_get_size (CLUTTER_ACTOR (viewport), &width, &height);

  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    value_changed = viewport_set_hadjustment_values (viewport, width);
  else
    value_changed = viewport_set_vadjustment_values (viewport, height);

  g_signal_connect (adjustment, "value-changed",
                    G_CALLBACK (viewport_adjustment_value_changed),
                    viewport);

  gtk_adjustment_changed (adjustment);

  if (value_changed)
    gtk_adjustment_value_changed (adjustment);
  else
    viewport_adjustment_value_changed (adjustment, viewport);

  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    g_object_notify (G_OBJECT (viewport), "hadjustment");
  else
    g_object_notify (G_OBJECT (viewport), "vadjustment");
}

static void
gtk_clutter_viewport_set_adjustments (GtkClutterScrollable *scrollable,
                                      GtkAdjustment        *h_adjust,
                                      GtkAdjustment        *v_adjust)
{
  g_object_freeze_notify (G_OBJECT (scrollable));

  connect_adjustment (GTK_CLUTTER_VIEWPORT (scrollable),
                      GTK_ORIENTATION_HORIZONTAL,
                      h_adjust);
  connect_adjustment (GTK_CLUTTER_VIEWPORT (scrollable),
                      GTK_ORIENTATION_VERTICAL,
                      v_adjust);

  g_object_thaw_notify (G_OBJECT (scrollable));
}

static void
gtk_clutter_viewport_get_adjustments (GtkClutterScrollable  *scrollable,
                                      GtkAdjustment        **h_adjust,
                                      GtkAdjustment        **v_adjust)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (scrollable)->priv;

  if (h_adjust)
    *h_adjust = priv->h_adjustment;

  if (v_adjust)
    *v_adjust = priv->v_adjustment;
}

static void
gtk_clutter_scrollable_iface_init (gpointer g_iface)
{
  GtkClutterScrollableIface *iface = g_iface;

  iface->set_adjustments = gtk_clutter_viewport_set_adjustments;
  iface->get_adjustments = gtk_clutter_viewport_get_adjustments;
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
                          GTK_ORIENTATION_HORIZONTAL,
                          g_value_get_object (value));
      break;

    case PROP_V_ADJUSTMENT:
      connect_adjustment (GTK_CLUTTER_VIEWPORT (gobject),
                          GTK_ORIENTATION_VERTICAL,
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
      g_value_set_object (value, priv->h_adjustment);
      break;

    case PROP_V_ADJUSTMENT:
      g_value_set_object (value, priv->v_adjustment);
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
                         GTK_ORIENTATION_HORIZONTAL);
  disconnect_adjustment (GTK_CLUTTER_VIEWPORT (gobject),
                         GTK_ORIENTATION_VERTICAL);

  G_OBJECT_CLASS (gtk_clutter_viewport_parent_class)->dispose (gobject);
}

static void
gtk_clutter_viewport_get_preferred_width (ClutterActor *actor,
                                          ClutterUnit   for_height,
                                          ClutterUnit  *min_width_p,
                                          ClutterUnit  *natural_width_p)
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
                                           ClutterUnit   for_width,
                                           ClutterUnit  *min_height_p,
                                           ClutterUnit  *natural_height_p)
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
gtk_clutter_viewport_allocate (ClutterActor          *actor,
                               const ClutterActorBox *box,
                               gboolean               origin_changed)
{
  GtkClutterViewport *viewport = GTK_CLUTTER_VIEWPORT (actor);
  GtkClutterViewportPrivate *priv = viewport->priv;
  ClutterActorClass *parent_class;
  gboolean h_adjustment_value_changed, v_adjustment_value_changed;
  guint width, height;

  parent_class = CLUTTER_ACTOR_CLASS (gtk_clutter_viewport_parent_class);
  parent_class->allocate (actor, box, origin_changed);

  width  = CLUTTER_UNITS_TO_DEVICE (box->x2 - box->x1);
  height = CLUTTER_UNITS_TO_DEVICE (box->y2 - box->y1);

  h_adjustment_value_changed =
    viewport_set_hadjustment_values (viewport, width);
  v_adjustment_value_changed =
    viewport_set_vadjustment_values (viewport, height);

  if (priv->child && CLUTTER_ACTOR_IS_VISIBLE (priv->child))
    {
      ClutterActorBox child_allocation = { 0, };
      ClutterUnit alloc_width, alloc_height;

      /* a viewport is a boundless actor which can contain a child
       * without constraints; hence, we give any child exactly the
       * wanted natural size, no matter how small the viewport
       * actually is.
       */
      clutter_actor_get_preferred_size (priv->child,
                                        NULL, NULL,
                                        &alloc_width, &alloc_height);

      child_allocation.x1 = clutter_actor_get_xu (priv->child);
      child_allocation.y1 = clutter_actor_get_yu (priv->child);
      child_allocation.x2 = child_allocation.x1 + alloc_width;
      child_allocation.y2 = child_allocation.y1 + alloc_height;

      clutter_actor_allocate (priv->child, &child_allocation, origin_changed);
    }

  gtk_adjustment_changed (priv->h_adjustment);
  gtk_adjustment_changed (priv->v_adjustment);

  if (h_adjustment_value_changed)
    gtk_adjustment_value_changed (priv->h_adjustment);

  if (v_adjustment_value_changed)
    gtk_adjustment_value_changed (priv->v_adjustment);
}

static void
gtk_clutter_viewport_paint (ClutterActor *actor)
{
  GtkClutterViewportPrivate *priv = GTK_CLUTTER_VIEWPORT (actor)->priv;

  cogl_push_matrix ();

  /* translate the paint environment by the same amount
   * defined by the origin value
   */
  cogl_translate (CLUTTER_UNITS_TO_FLOAT (priv->origin.x) * -1,
                  CLUTTER_UNITS_TO_FLOAT (priv->origin.y) * -1,
                  CLUTTER_UNITS_TO_FLOAT (priv->origin.z) * -1);

  /* the child will be painted in the new frame of reference */
  if (priv->child && CLUTTER_ACTOR_IS_VISIBLE (priv->child))
    clutter_actor_paint (priv->child);

  cogl_pop_matrix ();
}

static void
gtk_clutter_viewport_pick (ClutterActor       *actor,
                           const ClutterColor *pick_color)
{
  /* chain up to get the default pick */
  CLUTTER_ACTOR_CLASS (gtk_clutter_viewport_parent_class)->pick (actor, pick_color);

  /* this will cause the child (if any) to be painted in pick mode */
  gtk_clutter_viewport_paint (actor);
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
  actor_class->paint = gtk_clutter_viewport_paint;
  actor_class->pick = gtk_clutter_viewport_pick;

  /**
   * GtkClutterViewport:child:
   *
   * The #ClutterActor inside the viewport.
   *
   * Since: 1.0
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
   * The current origin of the viewport.
   *
   * Since: 1.0
   */
  pspec = g_param_spec_boxed ("origin",
                              "Origin",
                              "The current origin of the viewport",
                              CLUTTER_TYPE_VERTEX,
                              G_PARAM_READABLE);
  g_object_class_install_property (gobject_class, PROP_ORIGIN, pspec);

  /* GtkClutterScrollable properties */
  g_object_class_override_property (gobject_class, PROP_H_ADJUSTMENT, "hadjustment");
  g_object_class_override_property (gobject_class, PROP_V_ADJUSTMENT, "vadjustment");
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
 *
 * Creates a new #GtkClutterViewport with the given adjustments.
 *
 * Return value: the newly created viewport actor
 *
 * Since: 1.0
 */
ClutterActor *
gtk_clutter_viewport_new (GtkAdjustment *h_adjust,
                          GtkAdjustment *v_adjust)
{
  return g_object_new (GTK_CLUTTER_TYPE_VIEWPORT,
                       "hadjustment", h_adjust,
                       "vadjustment", v_adjust,
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
 * Since: 1.0.
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
    *x = CLUTTER_UNITS_TO_FLOAT (priv->origin.x);

  if (y)
    *y = CLUTTER_UNITS_TO_FLOAT (priv->origin.y);

  if (z)
    *z = CLUTTER_UNITS_TO_FLOAT (priv->origin.z);
}
