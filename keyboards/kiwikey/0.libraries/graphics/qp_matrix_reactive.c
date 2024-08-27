/*** MATRIX SOLID REACTIVE (without KEYMAP) ***/

#include "qp.h"
#include "qp_matrix_reactive.h"

extern painter_device_t my_display;

void render_solidmatrix_init(void) {
    for (uint8_t x = 0; x < MATRIX_ROWS-1; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS-1; y++) {
            qp_rect(my_display,
                    MATRIX_REACTIVE_DISPLAY_X + y*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING), // left
                    MATRIX_REACTIVE_DISPLAY_Y + x*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING), // top
                    MATRIX_REACTIVE_DISPLAY_X + y*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_WIDTH, // right
                    MATRIX_REACTIVE_DISPLAY_Y + x*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_HEIGHT, // bottom
                    255, // hue
                    0, // sat
                    255, // val
                    true);
        }
    }
}

void render_solidmatrix_update(uint8_t col, uint8_t row) {
	// char buf1[32] = {0};
    // sprintf(buf1, "Row %d Col %d", row, col);
	// qp_drawtext(my_display, 0, 30, arial30, buf1);
	
	bool on = (matrix_get_row(row) & (1 << col)) > 0; // The matrix position [x,y] is being pressed
	switch (row) {
		case 0 ... 3:
			if (!is_keyboard_left()) break;
			qp_rect(my_display,
					MATRIX_REACTIVE_DISPLAY_X + col*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING), // left
					MATRIX_REACTIVE_DISPLAY_Y + row*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING), // top
					MATRIX_REACTIVE_DISPLAY_X + col*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_WIDTH, // right
					MATRIX_REACTIVE_DISPLAY_Y + row*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_HEIGHT, // bottom
					255, // hue
					(on ? 255 : 0), // sat
					255, // val
					true);
			break;
		case 4 ... 7:
			if (is_keyboard_left()) break;
			qp_rect(my_display,
					MATRIX_REACTIVE_DISPLAY_X + (5-col)*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING), // left
					MATRIX_REACTIVE_DISPLAY_Y + (row-4)*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING), // top
					MATRIX_REACTIVE_DISPLAY_X + (5-col)*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_WIDTH, // right
					MATRIX_REACTIVE_DISPLAY_Y + (row-4)*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_HEIGHT, // bottom
					255, // hue
					(on ? 255 : 0), // sat
					255, // val
					true);
			break;
	}
}

// OLD CODE, FOR SPLIT SPORT48
// void render_solidmatrix_update(uint8_t col, uint8_t row) {
	// bool on = (matrix_get_row(row) & (1 << col)) > 0; // The matrix position [x,y] is being pressed
	// switch (row) {
		// case 0 ... 3:
			// if (!is_keyboard_left()) break;
			// qp_rect(my_display,
					// MATRIX_REACTIVE_DISPLAY_X + col*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING), // left
					// MATRIX_REACTIVE_DISPLAY_Y + row*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING), // top
					// MATRIX_REACTIVE_DISPLAY_X + col*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_WIDTH, // right
					// MATRIX_REACTIVE_DISPLAY_Y + row*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_HEIGHT, // bottom
					// 255, // hue
					// (on ? 255 : 0), // sat
					// 255, // val
					// true);
			// break;
		// case 4 ... 7:
			// if (is_keyboard_left()) break;
			// qp_rect(my_display,
					// MATRIX_REACTIVE_DISPLAY_X + (5-col)*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING), // left
					// MATRIX_REACTIVE_DISPLAY_Y + (row-4)*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING), // top
					// MATRIX_REACTIVE_DISPLAY_X + (5-col)*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_WIDTH, // right
					// MATRIX_REACTIVE_DISPLAY_Y + (row-4)*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_HEIGHT, // bottom
					// 255, // hue
					// (on ? 255 : 0), // sat
					// 255, // val
					// true);
			// break;
	// }
// }
/*** END OF MATRIX SOLID REACTIVE ***/