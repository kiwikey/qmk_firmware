#ifndef QP_WIDGET_LAYER
#define QP_WIDGET_LAYER

#include "quantum.h"

#define WIDGET_LAYER_POSX      5
#define WIDGET_LAYER_POSY      5
#define WIDGET_LAYER_SIZE      108

#define ACTIVE_LAYER_MIN    0
#define ACTIVE_LAYER_MAX    DYNAMIC_KEYMAP_LAYER_COUNT-1

extern bool qp_widget_layer_flag;

void widget_layer_init(void);
void widget_layer_render(uint8_t layer);

#endif /* QP_WIDGET_LAYER */
