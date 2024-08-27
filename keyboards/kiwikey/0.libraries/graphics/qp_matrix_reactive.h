#ifndef QP_MATRIX_REACTIVE
#define QP_MATRIX_REACTIVE

#include "quantum.h"

#define MATRIX_REACTIVE_DISPLAY_X    30
#define MATRIX_REACTIVE_DISPLAY_Y    120
#define MATRIX_REACTIVE_KEY_WIDTH    15
#define MATRIX_REACTIVE_KEY_HEIGHT   15
#define MATRIX_REACTIVE_KEY_SPACING  5

void render_solidmatrix_init(void);
void render_solidmatrix_update(uint8_t col, uint8_t row);

#endif /* QP_MATRIX_REACTIVE */