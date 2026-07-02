#pragma once

#ifndef QP_WIDGET_MATRIX
#define QP_WIDGET_MATRIX

#define WIDGET_MATRIX_POSX         10
#define WIDGET_MATRIX_POSY         50
#define WIDGET_MATRIX_KEY_WIDTH    40
#define WIDGET_MATRIX_KEY_HEIGHT   40
#define WIDGET_MATRIX_KEY_SPACING  5

#define WIDGET_MATRIX_LABLE_FONT      font_proggy_tiny
#define WIDGET_MATRIX_KC_BASIC_FONT   roboto20
#define WIDGET_MATRIX_KC_BASIC_COLOR  HSV_WHITE
#define WIDGET_MATRIX_KC_BASIC_BG     HSV_BLACK // should be get from widget background
#define WIDGET_MATRIX_LABEL_LAYER_BG  HSV_YELLOW

// #define WIDGET_MATRIX_BORDER  7

extern bool qp_widget_matrix_flag;  // This is not used

void widget_matrix_init(void);
void widget_matrix_update(uint8_t col, uint8_t row);

// RENDERING
void widget_matrix_draw_button(uint8_t x, uint8_t y, uint8_t hue, uint8_t sat, uint8_t val, bool filled);
void widget_matrix_keymap_render(void);
void widget_matrix_render_kc_layer(uint8_t posx, uint8_t posy, uint16_t keycode);
void widget_matrix_render_kc_basic(uint8_t posx, uint8_t posy, uint16_t keycode);

// OTHER FUNTIONS
char *keycode_to_string(enum qk_keycode_defines kc);

#endif /* QP_WIDGET_MATRIX */
