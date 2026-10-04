#pragma once

#include "quantum.h"
#include "display/defines.h"
#include "widget_layout.h" // WIDGET_MATRIX_POSX/POSY/KEY_*/WIDTH, shared with the layer-name box

#define WIDGET_MATRIX_KEY_CORNER   14

// #define WIDGET_MATRIX_KC_BASIC_FONT   nanoplex32
#define WIDGET_MATRIX_KC_BASIC_FONT   font16

// #define WIDGET_MATRIX_BG          0,0,40
#define WIDGET_MATRIX_BUTTON_BG   GLOBAL_BG_COLOR
#define WIDGET_MATRIX_BUTTON_OFF  eepdata.theme_hue, 255, 63
#define WIDGET_MATRIX_BUTTON_ON   eepdata.theme_hue, 255, 255
#define WIDGET_MATRIX_KC_COLOR    HSV_WHITE
#define WIDGET_MATRIX_KC_BG       WIDGET_MATRIX_BUTTON_BG
// #define WIDGET_MATRIX_LABEL_BG    HSV_YELLOW

void widget_matrix_init(void);
void widget_matrix_update(uint8_t col, uint8_t row);

// RENDERING
void widget_matrix_keymap_render(uint8_t layer);
void widget_matrix_render_kc_basic(uint16_t posx, uint16_t posy, uint16_t keycode);
void widget_matrix_render_kc_macro(uint16_t posx, uint16_t posy, uint16_t keycode);
void widget_matrix_render_kc_holdtap(uint16_t posx, uint16_t posy, uint16_t keycode);

void widget_matrix_bgclear_singlebutton(uint8_t x, uint8_t y);  // x and y are matrix [x,y], not pixel-related
void widget_matrix_render_singlebutton(uint8_t x, uint8_t y, uint8_t hue, uint8_t sat, uint8_t val, bool text_on, uint8_t layer);  // x and y are matrix [x,y], not pixel-related
// void widget_matrix_bgclear(void);

// OTHER FUNCTIONS
char *keycode_to_string(enum qk_keycode_defines kc);
