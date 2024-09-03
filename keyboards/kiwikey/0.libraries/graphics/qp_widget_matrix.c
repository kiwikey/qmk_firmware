/*** MATRIX SOLID REACTIVE (without KEYMAP) ***/

#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp.h"
#include "qp_widget_matrix.h"
#include "qp_custom_api.h"

extern painter_device_t my_display;

void widget_matrix_init(void) {
	// Background
	qp_roundrect(WIDGET_MATRIX_POSX - WIDGET_MATRIX_BORDER + WIDGET_LAYER_SHADOW,
				 WIDGET_MATRIX_POSY - WIDGET_MATRIX_BORDER + WIDGET_LAYER_SHADOW + 1,
				 WIDGET_MATRIX_POSX + 95+ WIDGET_MATRIX_BORDER + WIDGET_LAYER_SHADOW,
				 WIDGET_MATRIX_POSY + 95+ WIDGET_MATRIX_BORDER + WIDGET_LAYER_SHADOW,
				 0,0,100, true, 5, true, true);
	// Shadow
	qp_roundrect(WIDGET_MATRIX_POSX - WIDGET_MATRIX_BORDER,
				 WIDGET_MATRIX_POSY - WIDGET_MATRIX_BORDER + 1,
				 WIDGET_MATRIX_POSX + 95+ WIDGET_MATRIX_BORDER,
				 WIDGET_MATRIX_POSY + 95+ WIDGET_MATRIX_BORDER,
				 HSV_BLACK, true, 5, true, true);
	// Draw all keys
    for (uint8_t x = 0; x < MATRIX_ROWS; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS-1; y++) {
            qp_rect(my_display,
                    WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING), // left
                    WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING), // top
                    WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH, // right
                    WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT, // bottom
                    255, // hue
                    0, // sat
                    255, // val
                    true);
        }
    }
}

void widget_matrix_update(uint8_t col, uint8_t row) {
	// char buf1[32] = {0};
    // sprintf(buf1, "Row %d Col %d", row, col);
	// qp_drawtext(my_display, 0, 30, arial30, buf1);
	bool on = (matrix_get_row(row) & (1 << col)) > 0; // The matrix position [x,y] is being pressed
	qp_rect(my_display,
			WIDGET_MATRIX_POSX + col*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING), // left
			WIDGET_MATRIX_POSY + row*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING), // top
			WIDGET_MATRIX_POSX + col*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH, // right
			WIDGET_MATRIX_POSY + row*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT, // bottom
			255, // hue
			(on ? 255 : 0), // sat
			255, // val
			true);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
