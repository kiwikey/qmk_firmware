#pragma once

#define WIDGET_MATRIX_POSX         10
#define WIDGET_MATRIX_POSY         50
#define WIDGET_MATRIX_KEY_WIDTH    40
#define WIDGET_MATRIX_KEY_HEIGHT   40
#define WIDGET_MATRIX_KEY_SPACING  5
#define WIDGET_MATRIX_WIDTH        (WIDGET_MATRIX_KEY_WIDTH*4 + WIDGET_MATRIX_KEY_SPACING*3)
#define WIDGET_MATRIX_BORDER       5 // Widget inner thickness

#define WIDGET_MATRIX_BTN1_POSX    225
#define WIDGET_MATRIX_BTN1_POSY    210
#define WIDGET_MATRIX_BTN2_POSX    285
#define WIDGET_MATRIX_BTN2_POSY    WIDGET_MATRIX_BTN1_POSY
#define WIDGET_MATRIX_BTN_RADIUS   20

#define WIDGET_MATRIX_LABEL_FONT      font_proggy_tiny
#define WIDGET_MATRIX_KC_BASIC_FONT   roboto20

#define WIDGET_MATRIX_BG          0,0,40
#define WIDGET_MATRIX_BUTTON_BG   HSV_BLACK
#define WIDGET_MATRIX_BUTTON_OFF  HSV_WHITE
#define WIDGET_MATRIX_BUTTON_ON   HSV_RED
#define WIDGET_MATRIX_KC_COLOR    HSV_WHITE
#define WIDGET_MATRIX_KC_BG       HSV_BLACK // should be get from widget background
#define WIDGET_MATRIX_LABEL_BG    HSV_YELLOW

// #define WIDGET_MATRIX_BTN_COLOR   HSV_WHITE

void widget_matrix_init(void);
void widget_matrix_update(uint8_t col, uint8_t row);

// RENDERING
void widget_matrix_keymap_render(uint8_t layer);
void widget_matrix_render_kc_layer(uint16_t posx, uint16_t posy, uint16_t keycode);
void widget_matrix_render_kc_basic(uint16_t posx, uint16_t posy, uint16_t keycode);

void widget_matrix_bgclear_singlebutton(uint8_t x, uint8_t y);  // x and y are matrix [x,y], not pixel-related
void widget_matrix_render_singlebutton(uint8_t x, uint8_t y, uint8_t hue, uint8_t sat, uint8_t val, bool filled);  // x and y are matrix [x,y], not pixel-related
void widget_matrix_bgclear(void);

// OTHER FUNCTIONS
char *keycode_to_string(enum qk_keycode_defines kc);
