#ifndef QP_WIDGET_LAYER
#define QP_WIDGET_LAYER

#include "quantum.h"

#define WIDGET_LAYER_POSX      5
#define WIDGET_LAYER_POSY      5
#define WIDGET_LAYER_SIZE      108
#define WIDGET_LAYER_TEXTCOLOR HSV_BLACK
#define WIDGET_LAYER_SHADOW    3 // shadow of widgets, in pixel

static const char * const layer_name[DYNAMIC_KEYMAP_LAYER_COUNT] = {
	"Default",
	" Raise ",
	" Lower ",
	" User  "
};

void widget_layer_init(void);
void widget_layer_render(uint8_t layer);

#endif /* QP_WIDGET_LAYER */
