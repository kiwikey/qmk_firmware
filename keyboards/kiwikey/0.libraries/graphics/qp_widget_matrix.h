#pragma once

#ifndef QP_WIDGET_MATRIX
#define QP_WIDGET_MATRIX

#define WIDGET_MATRIX_POSX         132
#define WIDGET_MATRIX_POSY         12
#define WIDGET_MATRIX_KEY_WIDTH    15
#define WIDGET_MATRIX_KEY_HEIGHT   15
#define WIDGET_MATRIX_KEY_SPACING  5

#define WIDGET_MATRIX_BORDER  7
#define WIDGET_LAYER_SHADOW        3 // shadow of widgets, in pixel

void widget_matrix_init(void);
void widget_matrix_update(uint8_t col, uint8_t row);

#endif /* QP_WIDGET_MATRIX */
