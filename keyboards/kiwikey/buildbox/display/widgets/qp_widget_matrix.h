#pragma once

#define WIDGET_MATRIX_POSX         WIDGET_LAYER_POSX
#define WIDGET_MATRIX_POSY         WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT + 10
#define WIDGET_MATRIX_KEY_WIDTH    50
#define WIDGET_MATRIX_KEY_HEIGHT   50
#define WIDGET_MATRIX_KEY_CORNER   14
#define WIDGET_MATRIX_KEY_SPACING  1
#define WIDGET_MATRIX_WIDTH        (WIDGET_MATRIX_KEY_WIDTH*4 + WIDGET_MATRIX_KEY_SPACING*3)

// #define WIDGET_MATRIX_LABEL_FONT      thintel16
#define WIDGET_MATRIX_KC_BASIC_FONT   nanoplex32

// #define WIDGET_MATRIX_BG          0,0,40
#define WIDGET_MATRIX_BUTTON_BG   GLOBAL_BG_COLOR
#define WIDGET_MATRIX_BUTTON_OFF  GLOBAL_THEME_COLOR
// Complementary hue (opposite side of the wheel, wraps via uint8_t overflow)
// instead of a fixed HSV_YELLOW - guarantees contrast against
// WIDGET_MATRIX_BUTTON_OFF (GLOBAL_THEME_COLOR) no matter what hue the user
// picks in the Settings Menu, instead of the two matching when theme_hue
// happens to land near yellow.
#define WIDGET_MATRIX_BUTTON_ON   eepdata.theme_hue + 128, 255, 255
#define WIDGET_MATRIX_KC_COLOR    HSV_WHITE
#define WIDGET_MATRIX_KC_BG       WIDGET_MATRIX_BUTTON_BG
// #define WIDGET_MATRIX_LABEL_BG    HSV_YELLOW

void widget_matrix_init(void);
void widget_matrix_update(uint8_t col, uint8_t row);

// RENDERING
void widget_matrix_keymap_render(uint8_t layer);
void widget_matrix_render_kc_layer(uint16_t posx, uint16_t posy, uint16_t keycode);
void widget_matrix_render_kc_basic(uint16_t posx, uint16_t posy, uint16_t keycode);

void widget_matrix_bgclear_singlebutton(uint8_t x, uint8_t y);  // x and y are matrix [x,y], not pixel-related
void widget_matrix_render_singlebutton(uint8_t x, uint8_t y, uint8_t hue, uint8_t sat, uint8_t val, bool text_on, uint8_t layer);  // x and y are matrix [x,y], not pixel-related
// void widget_matrix_bgclear(void);

// OTHER FUNCTIONS
char *keycode_to_string(enum qk_keycode_defines kc);
