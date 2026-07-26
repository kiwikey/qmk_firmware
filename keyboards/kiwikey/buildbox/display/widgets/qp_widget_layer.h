#pragma once

#include "qp_widget_matrix.h" // getting some defines from WIDGET_MATRIX

#define WIDGET_LAYER_POSX      5
#define WIDGET_LAYER_POSY      5
#define WIDGET_LAYER_WIDTH     (WIDGET_MATRIX_KEY_WIDTH * 4)
#define WIDGET_LAYER_HEIGHT    25
#define WIDGET_LAYER_CORNER    5

#define WIDGET_LAYER_OUTLINE   HSV_WHITE
#define WIDGET_LAYER_TEXT      HSV_BLACK
#define WIDGET_LAYER_BG        HSV_WHITE
#define WIDGET_LAYER_ON_TEXT   HSV_BLACK // NOT USED
#define WIDGET_LAYER_OFF_TEXT  HSV_WHITE // NOT USED

#define WIDGET_LAYER_NAV_POSX  220
#define WIDGET_LAYER_NAV_POSY  200

#define WIDGET_LAYER_FONT      nanoplex32
#define WIDGET_LAYER_NAV_FONT  nanoplex16

void widget_layer_init(void);
// void widget_layer_render_layernumber(uint8_t layer);
void widget_layer_render_layername(uint8_t layer);
void widget_layer_render_navigation(uint8_t layer);
