#pragma once

#include "qp_widget_matrix.h" // getting some defines from WIDGET_MATRIX

#define WIDGET_LAYER_WIDTH     80
#define WIDGET_LAYER_HEIGHT    30
#define WIDGET_LAYER_POSX      WIDGET_MATRIX_POSX
#define WIDGET_LAYER_POSY      (WIDGET_MATRIX_POSY - WIDGET_LAYER_HEIGHT - 5)

#define WIDGET_LAYER_OUTLINE   HSV_WHITE
#define WIDGET_LAYER_BG        HSV_BLACK
#define WIDGET_LAYER_ON_TEXT   HSV_BLACK
#define WIDGET_LAYER_OFF_TEXT  HSV_WHITE

// #define ACTIVE_LAYER_MIN    0
// #define ACTIVE_LAYER_MAX    (DYNAMIC_KEYMAP_LAYER_COUNT-1) // TODO: -1 applied to Kiwi5x5 only

void widget_layer_init(void);
void widget_layer_render(uint8_t layer);
void widget_layer_render_layername(uint8_t layer);
