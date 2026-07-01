#pragma once

#ifndef QP_WIDGET_MATRIX
#define QP_WIDGET_MATRIX

#define WIDGET_MATRIX_POSX         10
#define WIDGET_MATRIX_POSY         50
#define WIDGET_MATRIX_KEY_WIDTH    40
#define WIDGET_MATRIX_KEY_HEIGHT   40
#define WIDGET_MATRIX_KEY_SPACING  5

// #define WIDGET_MATRIX_BORDER  7

extern bool qp_widget_matrix_flag;  // This is not used

void widget_matrix_init(void);
void widget_matrix_update(uint8_t col, uint8_t row); // This is not used
void widget_matrix_keymap_render(void);
char *keycode_to_string(enum qk_keycode_defines kc);

#endif /* QP_WIDGET_MATRIX */
