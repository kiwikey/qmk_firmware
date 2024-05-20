// Configuration for Key Matrix
#define MATRIX_DISPLAY_X    107
#define MATRIX_DISPLAY_Y    11
#define MATRIX_DISPLAY_SIZE 22
#define KEY_SIZE            4
#define KEY_SPACING         3

void render_matrix(void) {
    for (uint8_t x = 0; x < MATRIX_ROWS-2; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS-2; y++) {
            bool on = (matrix_get_row(x) & (1 << y)) > 0; // The matrix position [x,y] is being pressed
            draw_rect( MATRIX_DISPLAY_X + y*(KEY_SIZE+KEY_SPACING), // X - horizontal position of key
                       MATRIX_DISPLAY_Y + x*(KEY_SIZE+KEY_SPACING), // Y - vertical position of key
                       KEY_SIZE,
                       KEY_SIZE,
                       on);
        }
    }
}
