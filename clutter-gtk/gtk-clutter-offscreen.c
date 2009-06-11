/*
 * gtkclutteroffscreen.c
 */

#include "config.h"

#include <gtk/gtk.h>

#include "gtk-clutter-offscreen.h"
#include "gtk-clutter-actor-internal.h"

static void        gtk_clutter_offscreen_realize       (GtkWidget       *widget);
static void        gtk_clutter_offscreen_unrealize     (GtkWidget       *widget);
static void        gtk_clutter_offscreen_size_request  (GtkWidget       *widget,
							GtkRequisition  *requisition);
static void        gtk_clutter_offscreen_size_allocate (GtkWidget       *widget,
							GtkAllocation   *allocation);
static gboolean    gtk_clutter_offscreen_damage        (GtkWidget       *widget,
							GdkEventExpose  *event);

G_DEFINE_TYPE (GtkClutterOffscreen, gtk_clutter_offscreen, GTK_TYPE_BIN);

static gint
gtk_clutter_offscreen_expose (GtkWidget      *widget,
			      GdkEventExpose *event)
{
  return GTK_WIDGET_CLASS (gtk_clutter_offscreen_parent_class)->expose_event (widget, event);

}

static void
gtk_clutter_offscreen_check_resize (GtkContainer *container)
{
  GtkClutterOffscreen *offscreen = GTK_CLUTTER_OFFSCREEN (container);

  clutter_actor_queue_relayout (offscreen->actor);
}

static void
gtk_clutter_offscreen_class_init (GtkClutterOffscreenClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  widget_class->expose_event = gtk_clutter_offscreen_expose;
  widget_class->realize = gtk_clutter_offscreen_realize;
  widget_class->unrealize = gtk_clutter_offscreen_unrealize;
  widget_class->size_request = gtk_clutter_offscreen_size_request;
  widget_class->size_allocate = gtk_clutter_offscreen_size_allocate;

  container_class->check_resize = gtk_clutter_offscreen_check_resize;

  g_signal_override_class_closure (g_signal_lookup ("damage-event", GTK_TYPE_WIDGET),
				   GTK_TYPE_CLUTTER_OFFSCREEN,
				   g_cclosure_new (G_CALLBACK (gtk_clutter_offscreen_damage),
						   NULL, NULL));
}

static void
gtk_clutter_offscreen_init (GtkClutterOffscreen *offscreen)
{
  GTK_WIDGET_UNSET_FLAGS (offscreen, GTK_NO_WINDOW);
  gtk_container_set_resize_mode (GTK_CONTAINER (offscreen), GTK_RESIZE_IMMEDIATE);
}

GtkWidget *
gtk_clutter_offscreen_new (ClutterActor *actor)
{
  GtkClutterOffscreen *offscreen;

  offscreen = g_object_new (GTK_TYPE_CLUTTER_OFFSCREEN, NULL);
  offscreen->actor = actor; /* Back pointer, actor owns widget */
  return GTK_WIDGET (offscreen);
}

GdkWindow *
get_embedding_window (GtkClutterOffscreen *offscreen)
{
  GtkWidget *embed;
  embed = _gtk_clutter_actor_get_embed (GTK_CLUTTER_ACTOR (offscreen->actor));
  return embed->window;
}

static GdkWindow *
get_offscreen_parent (GdkWindow *offscreen_window,
		      GtkClutterOffscreen *offscreen)
{
  return get_embedding_window (offscreen);
}

static void
offscreen_window_to_parent (GdkWindow       *offscreen_window,
			    double           offscreen_x,
			    double           offscreen_y,
			    double          *parent_x,
			    double          *parent_y,
			    GtkClutterOffscreen *offscreen)
{
  ClutterVertex point, vertex;

  point.x = offscreen_x;
  point.y = offscreen_y;
  point.z = 0;
  clutter_actor_apply_transform_to_point (offscreen->actor, &point, &vertex);
  *parent_x = vertex.x;
  *parent_y = vertex.y;
}

static void
offscreen_window_from_parent (GdkWindow       *window,
			      double           parent_x,
			      double           parent_y,
			      double          *offscreen_x,
			      double          *offscreen_y,
			      GtkClutterOffscreen *offscreen)
{
  *offscreen_x = parent_x;
  *offscreen_y = parent_y;
}

static void
gtk_clutter_offscreen_realize (GtkWidget *widget)
{
  GtkClutterOffscreen *offscreen = GTK_CLUTTER_OFFSCREEN (widget);
  GdkWindowAttr attributes;
  gint attributes_mask;
  gint border_width;
  GdkWindow *parent;

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  border_width = GTK_CONTAINER (widget)->border_width;

  attributes.x = widget->allocation.x + border_width;
  attributes.y = widget->allocation.y + border_width;
  attributes.width = widget->allocation.width - 2 * border_width;
  attributes.height = widget->allocation.height - 2 * border_width;
  attributes.window_type = GDK_WINDOW_OFFSCREEN;
  attributes.event_mask = gtk_widget_get_events (widget) |
    GDK_EXPOSURE_MASK;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.wclass = GDK_INPUT_OUTPUT;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  parent = get_embedding_window (offscreen);

  widget->window = gdk_window_new (gdk_screen_get_root_window (gdk_drawable_get_screen (GDK_DRAWABLE (parent))),
				   &attributes, attributes_mask);
  gdk_window_set_user_data (widget->window, widget);

  g_signal_connect (widget->window, "get-offscreen-parent",
		    G_CALLBACK (get_offscreen_parent), widget);
  g_signal_connect (widget->window, "to_parent",
		    G_CALLBACK (offscreen_window_to_parent), widget);
  g_signal_connect (widget->window, "from_parent",
		    G_CALLBACK (offscreen_window_from_parent), widget);

  widget->style = gtk_style_attach (widget->style, widget->window);

  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
}

static void
gtk_clutter_offscreen_unrealize (GtkWidget *widget)
{
  GTK_WIDGET_CLASS (gtk_clutter_offscreen_parent_class)->unrealize (widget);
}

static void
gtk_clutter_offscreen_size_request (GtkWidget      *widget,
				    GtkRequisition *requisition)
{
  requisition->width = (GTK_CONTAINER (widget)->border_width * 2);
  requisition->height = (GTK_CONTAINER (widget)->border_width * 2);

  if (GTK_BIN (widget)->child && GTK_WIDGET_VISIBLE (GTK_BIN (widget)->child))
    {
      GtkRequisition child_requisition;

      gtk_widget_size_request (GTK_BIN (widget)->child, &child_requisition);

      requisition->width += child_requisition.width;
      requisition->height += child_requisition.height;
    }
}

static void
gtk_clutter_offscreen_size_allocate (GtkWidget     *widget,
				     GtkAllocation *allocation)
{
  GtkClutterOffscreen *offscreen;
  gint border_width;

  widget->allocation = *allocation;
  offscreen = GTK_CLUTTER_OFFSCREEN (widget);

  border_width = GTK_CONTAINER (widget)->border_width;

  if (GTK_WIDGET_REALIZED (widget))
    gdk_window_move_resize (widget->window,
			    0, 0,
			    allocation->width,
			    allocation->height);

  if (GTK_BIN (offscreen)->child && GTK_WIDGET_VISIBLE (GTK_BIN (offscreen)->child))
    {
      GtkAllocation child_allocation;

      child_allocation.x = border_width;
      child_allocation.y = border_width;

      child_allocation.width = MAX (1, widget->allocation.width -
				    border_width * 2);
      child_allocation.height = MAX (1, widget->allocation.height -
				     border_width * 2);

      gtk_widget_size_allocate (GTK_BIN (widget)->child, &child_allocation);
    }
}

static gboolean
gtk_clutter_offscreen_damage (GtkWidget      *widget,
			      GdkEventExpose *event)
{
  GtkClutterOffscreen *offscreen = GTK_CLUTTER_OFFSCREEN (widget);

  _gtk_clutter_actor_update (GTK_CLUTTER_ACTOR (offscreen->actor),
			     event->area.x,
			     event->area.y,
			     event->area.width,
			     event->area.height);

  return TRUE;
}
