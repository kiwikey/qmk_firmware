#ifndef QP_WIDGET_LAYER
#define QP_WIDGET_LAYER

#include "quantum.h"
#include "qp_widget_matrix.h"

#define WIDGET_LAYER_POSX      WIDGET_MATRIX_POSX
#define WIDGET_LAYER_POSY      (WIDGET_MATRIX_POSY - WIDGET_LAYER_HEIGHT)
#define WIDGET_LAYER_WIDTH     80
#define WIDGET_LAYER_HEIGHT    30

#define ACTIVE_LAYER_MIN    0
#define ACTIVE_LAYER_MAX    (DYNAMIC_KEYMAP_LAYER_COUNT-1) // TODO: -1 applied to Kiwi5x5 only

extern bool qp_widget_layer_flag;

void widget_layer_init(void);
void widget_layer_render(uint8_t layer);

#endif /* QP_WIDGET_LAYER */
