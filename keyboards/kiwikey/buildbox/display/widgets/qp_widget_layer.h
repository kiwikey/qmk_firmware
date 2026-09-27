#pragma once

#include "qp_widget_matrix.h" // getting some defines from WIDGET_MATRIX

#define WIDGET_LAYER_POSX      5
#define WIDGET_LAYER_POSY      0
#define WIDGET_LAYER_WIDTH     (WIDGET_MATRIX_KEY_WIDTH*4 + WIDGET_MATRIX_KEY_SPACING*3)
#define WIDGET_LAYER_HEIGHT    25
#define WIDGET_LAYER_CORNER    5

#define WIDGET_LAYER_TEXT         HSV_BLACK
#define WIDGET_LAYER_BG           HSV_WHITE

#define WIDGET_LAYER_ICON_PADDING 6  // gap between the per-layer icon and the box's right edge

#define WIDGET_LAYER_NAV_PADDING  15
#define WIDGET_LAYER_NAV_POSX1    (WIDGET_LAYER_POSX + WIDGET_MATRIX_KEY_WIDTH*4 + WIDGET_MATRIX_KEY_SPACING*3 + WIDGET_LAYER_NAV_PADDING)
#define WIDGET_LAYER_NAV_POSY1    200
#define WIDGET_LAYER_NAV_POSX2    (DISPLAY_WIDTH - ico12_arrow_right->width - WIDGET_LAYER_NAV_PADDING)
#define WIDGET_LAYER_NAV_POSY2    220

#define WIDGET_LAYER_FONT      nanoplex32
#define WIDGET_LAYER_NAV_FONT  font16

void widget_layer_init(void);
void widget_layer_render_layername(uint8_t layer, uint16_t posx, uint16_t posy); // posx/posy let callers (e.g. the "LAYERS CONFIG" menu) stack several of these without colliding
void widget_layer_render_navigation(uint8_t layer);

// Per-layer icon (drawn inside the box by widget_layer_render_layername() above)
// - single source of truth shared with the "LAYERS CONFIG" icon picker
// (qp_menu.c), so a choice made there is reflected everywhere the box is drawn.
#define LAYER_ICON_POOL_COUNT 24 // display/resources/icons/22x22/ - see layer_icon_pool_icon()'s definition (qp_widget_layer.c)

painter_image_handle_t layer_icon_pool_icon(uint8_t pool_index);            // pool_index wraps % LAYER_ICON_POOL_COUNT
uint8_t                layer_icon_get_choice(uint8_t layer);                // currently chosen pool index for that layer
void                    layer_icon_set_choice(uint8_t layer, uint8_t pool_index); // called when the picker saves a new choice
