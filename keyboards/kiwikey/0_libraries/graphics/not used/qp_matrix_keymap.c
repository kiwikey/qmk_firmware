#include "qp_matrix_keymap.h"

char buf[20] = {0};

const char code_to_name[60] = {
    // ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
    // 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    // 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    // '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    // 'R', 'E', 'B', 'T', ' ', ' ', ' ', ' ', ' ', ' ',
    // ' ', ';', '\'', ' ', ',', '.', '/', ' ', ' ', ' '
	
	' ', ' ', ' ', ' ', 'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ';', '\'', ' ', ',', '.', '/', ' ', ' ', ' '
};

void render_matrix_left(void) {
    for (uint8_t x = 0; x < MATRIX_ROWS/2; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS-1; y++) {
            bool on = (matrix_get_row(x) & (1 << y)) > 0; // The matrix position [x,y] is being pressed
            // qp_rect(my_display,
                    // MATRIX_DISPLAY_X -6 + y*(KEY_WIDTH  + KEY_SPACING), // left
                    // MATRIX_DISPLAY_Y -6 + x*(KEY_HEIGHT + KEY_SPACING), // top
                    // MATRIX_DISPLAY_X +4 + y*(KEY_WIDTH  + KEY_SPACING) + KEY_WIDTH, // right
                    // MATRIX_DISPLAY_Y +4 + x*(KEY_HEIGHT + KEY_SPACING) + KEY_HEIGHT, // bottom
                    // 255, // hue
                    // (on ? 255 : 0), // sat
                    // 255, // val
                    // false);
			uint16_t keycode = keymap_key_to_keycode(0, (keypos_t){y,x});
			snprintf(buf, sizeof(buf), "%c", code_to_name[keycode]);
			qp_drawtext_recolor(my_display,
								MATRIX_DISPLAY_X + y*(KEY_WIDTH + KEY_SPACING),
								MATRIX_DISPLAY_Y + x*(KEY_HEIGHT + KEY_SPACING),
								arial30, buf,
								255, (on ? 255 : 0), 255, // text color (HUE, SAT, VAL)
								255, 0, 0 // background color (HUE, SAT, VAL)
								);
        }
    }
}

void render_matrix_right(void) {
    for (uint8_t x = MATRIX_ROWS/2; x < MATRIX_ROWS; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS-1; y++) {
            bool on = (matrix_get_row(x) & (1 << y)) > 0; // The matrix position [x,y] is being pressed
            // qp_rect(my_display,
                    // MATRIX_DISPLAY_X -8 + y*(KEY_WIDTH  + KEY_SPACING), // left
                    // MATRIX_DISPLAY_Y -5 + (x-4)*(KEY_HEIGHT + KEY_SPACING), // top
                    // MATRIX_DISPLAY_X + y*(KEY_WIDTH  + KEY_SPACING) + KEY_WIDTH, // right
                    // MATRIX_DISPLAY_Y + (x-4)*(KEY_HEIGHT + KEY_SPACING) + KEY_HEIGHT, // bottom
                    // 255, // hue
                    // (on ? 255 : 0), // sat
                    // 255, // val
                    // false);
			uint16_t keycode = keymap_key_to_keycode(0, (keypos_t){y,x});
			snprintf(buf, sizeof(buf), "%c", code_to_name[keycode]);
			qp_drawtext_recolor(my_display,
								MATRIX_DISPLAY_X + (5-y)*(KEY_WIDTH + KEY_SPACING),
								MATRIX_DISPLAY_Y + (x-4)*(KEY_HEIGHT + KEY_SPACING),
								arial30, buf,
								255, (on ? 255 : 0), 255, // text color (HUE, SAT, VAL)
								255, 0, 0 // background color (HUE, SAT, VAL)
								);
        }
    }
}

void render_matrix_left_update(uint8_t col, uint8_t row) {
	bool on = (matrix_get_row(row) & (1 << col)) > 0; // The matrix position [x,y] is being pressed
	uint16_t keycode = keymap_key_to_keycode(0, (keypos_t){col,row});
	snprintf(buf, sizeof(buf), "%c", code_to_name[keycode]);
	qp_drawtext_recolor(my_display,
						MATRIX_DISPLAY_X + col*(KEY_WIDTH + KEY_SPACING),
						MATRIX_DISPLAY_Y + row*(KEY_HEIGHT + KEY_SPACING),
						arial30, buf,
						255, (on ? 255 : 0), 255, // text color (HUE, SAT, VAL)
						255, 0, 0 // background color (HUE, SAT, VAL)
						);
}

void render_matrix_right_update(uint8_t col, uint8_t row) {
	bool on = (matrix_get_row(row) & (1 << col)) > 0; // The matrix position [x,y] is being pressed
	uint16_t keycode = keymap_key_to_keycode(0, (keypos_t){col,row});
	snprintf(buf, sizeof(buf), "%c", code_to_name[keycode]);
	qp_drawtext_recolor(my_display,
						MATRIX_DISPLAY_X + (5-col)*(KEY_WIDTH + KEY_SPACING),
						MATRIX_DISPLAY_Y + (row-4)*(KEY_HEIGHT + KEY_SPACING),
						arial30, buf,
						255, (on ? 255 : 0), 255, // text color (HUE, SAT, VAL)
						255, 0, 0 // background color (HUE, SAT, VAL)
						);
}
