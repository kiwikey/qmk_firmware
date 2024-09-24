/*** MATRIX SOLID REACTIVE (without KEYMAP) ***/

#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp.h"
#include "qp_widget_matrix.h"
#include "qp_custom_api.h"
#include "defines.h"

extern painter_device_t my_display;
bool   qp_widget_matrix_flag = false;

void widget_matrix_init(void) {
	// Shadow
	qp_roundrect(WIDGET_MATRIX_POSX - WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW,
				 WIDGET_MATRIX_POSY - WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW + 1,
				 WIDGET_MATRIX_POSX + 95 + WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW,
				 WIDGET_MATRIX_POSY + 95 + WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW,
				 UI_COLOR_SHADOW, true, 5, true, true);
	// Background
	qp_roundrect(WIDGET_MATRIX_POSX - WIDGET_MATRIX_BORDER,
				 WIDGET_MATRIX_POSY - WIDGET_MATRIX_BORDER + 1,
				 WIDGET_MATRIX_POSX + 95 + WIDGET_MATRIX_BORDER,
				 WIDGET_MATRIX_POSY + 95 + WIDGET_MATRIX_BORDER,
				 WIDGET_MATRIX_BACKGROUND, true, 5, true, true);
	// Draw all keys
    for (uint8_t x = 0; x < 5; x++) { // MATRIX_ROWS
        for (uint8_t y = 0; y < 5; y++) { // MATRIX_COLS
            qp_rect(my_display,
                    WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING), // left
                    WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING), // top
                    WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH, // right
                    WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT, // bottom
					WIDGET_MATRIX_COLOR_OFF,
                    true);
        }
    }
}

void widget_matrix_update(uint8_t col, uint8_t row) {
	if (col != 5) { // TODO: hotfix for knob keypress
		/* This line is for Kiwi5x5, which uses Swap-hands to rotate layout 90degree clockwise */
		/* TODO: add normal calculation for normal uses */
		bool on = (is_swap_hands_on() ? matrix_is_on(col, 4-row) : matrix_is_on(row, col)); // magical coding, again
		// bool on = (matrix_get_row(row) & (1 << col)) > 0; // The matrix position [x,y] is being pressed
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
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
