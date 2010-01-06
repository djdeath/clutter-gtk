#include "gtk-clutter-standin-bin.h"
#include "gtk-clutter-standin.h"

G_BEGIN_DECLS

void _gtk_clutter_standin_bin_gtk_size_request  (GtkClutterStandinBin *self,
                                                 GtkRequisition       *requisition);
void _gtk_clutter_standin_bin_gtk_size_allocate (GtkClutterStandinBin *self,
                                                 GtkAllocation        *allocation);

G_END_DECLS
