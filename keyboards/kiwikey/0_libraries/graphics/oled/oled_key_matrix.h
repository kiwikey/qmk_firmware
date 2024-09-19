#ifndef OLED_KEY_MATRIX
#define OLED_KEY_MATRIX

#include "quantum.h"

// Configuration for Key Matrix
#define MATRIX_DISPLAY_X    85
#define MATRIX_DISPLAY_Y    10
#define MATRIX_DISPLAY_SIZE 22
#define KEY_SIZE            4
#define KEY_SPACING         3

void render_matrix(void);

#endif /* OLED_KEY_MATRIX */
