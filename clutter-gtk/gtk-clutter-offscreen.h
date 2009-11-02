#ifndef __GTK_CLUTTER_OFFSCREEN_H__
#define __GTK_CLUTTER_OFFSCREEN_H__

#include <gtk/gtk.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GTK_TYPE_CLUTTER_OFFSCREEN              (gtk_clutter_offscreen_get_type ())
#define GTK_CLUTTER_OFFSCREEN(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_CLUTTER_OFFSCREEN, GtkClutterOffscreen))
#define GTK_CLUTTER_OFFSCREEN_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_CLUTTER_OFFSCREEN, GtkClutterOffscreenClass))
#define GTK_IS_CLUTTER_OFFSCREEN(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_CLUTTER_OFFSCREEN))
#define GTK_IS_CLUTTER_OFFSCREEN_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_CLUTTER_OFFSCREEN))
#define GTK_CLUTTER_OFFSCREEN_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_CLUTTER_OFFSCREEN, GtkClutterOffscreenClass))

typedef struct _GtkClutterOffscreen	  GtkClutterOffscreen;
typedef struct _GtkClutterOffscreenClass  GtkClutterOffscreenClass;

struct _GtkClutterOffscreen
{
  GtkBin bin;

  gboolean active;
  ClutterActor *actor;
};

struct _GtkClutterOffscreenClass
{
  GtkBinClass parent_class;
};

GType	   gtk_clutter_offscreen_get_type   (void) G_GNUC_CONST;
GtkWidget* gtk_clutter_offscreen_new        (ClutterActor *actor);
void       gtk_clutter_offscreen_set_active (GtkClutterOffscreen *offscreen,
					     gboolean      active);

G_END_DECLS

#endif /* __GTK_CLUTTER_OFFSCREEN_H__ */
