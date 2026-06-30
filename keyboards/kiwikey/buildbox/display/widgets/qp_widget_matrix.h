#pragma once

#ifndef QP_WIDGET_MATRIX
#define QP_WIDGET_MATRIX

#define WIDGET_MATRIX_POSX         0
#define WIDGET_MATRIX_POSY         0
#define WIDGET_MATRIX_KEY_WIDTH    40
#define WIDGET_MATRIX_KEY_HEIGHT   40
#define WIDGET_MATRIX_KEY_SPACING  5

#define WIDGET_MATRIX_BORDER  7

extern bool qp_widget_matrix_flag;

void widget_matrix_init(void);
void widget_matrix_update(uint8_t col, uint8_t row);
void widget_matrix_keymap_render(void);

#endif /* QP_WIDGET_MATRIX */
