#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
  char *label;
  int selected;
} MenuItem;

void drawMenu(Display *display, Window w, int width, int height,
              MenuItem *items, int items_count, GC gc, GC selected_gc);
void handleSubmenuSelection(int selected_index);
void initGui();
