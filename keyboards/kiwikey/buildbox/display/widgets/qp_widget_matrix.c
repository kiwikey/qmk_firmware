#if defined(QUANTUM_PAINTER_ENABLE)

#include <qp.h>
#include "qp_widget_matrix.h"
#include "../qp_custom_api.h"
#include "../defines.h"
#include "keycodes_list.h"

#if defined(QUANTUM_PAINTER_ENABLE)
	#include "display/qp_includes.h"
	#include "display/eeprom_custom.h"
	#include "display/widgets/qp_widget_matrix.h"
	#include "display/widgets/qp_widget_layer.h"
	#include "display/widgets/qp_widget_knob.h"
	painter_device_t my_display;
#endif // defined(QUANTUM_PAINTER_ENABLE)

void widget_matrix_init(void) {
	// Draw shadow
	// qp_roundrect(WIDGET_MATRIX_POSX - WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW,
				 // WIDGET_MATRIX_POSY - WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW + 1,
				 // WIDGET_MATRIX_POSX + 95 + WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW,
				 // WIDGET_MATRIX_POSY + 95 + WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW,
				 // UI_COLOR_SHADOW, true, 5, true, true);
	// Draw background
	// qp_roundrect(WIDGET_MATRIX_POSX - WIDGET_MATRIX_BORDER,
				 // WIDGET_MATRIX_POSY - WIDGET_MATRIX_BORDER + 1,
				 // WIDGET_MATRIX_POSX + (WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING)*4 - WIDGET_MATRIX_KEY_SPACING + WIDGET_MATRIX_BORDER,
				 // WIDGET_MATRIX_POSY + (WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING)*4 - WIDGET_MATRIX_KEY_SPACING + WIDGET_MATRIX_BORDER,
				 // WIDGET_MATRIX_BACKGROUND, true, 5, true, true);
	// Draw all buttons
    for (uint8_t x = 0; x < MATRIX_ROWS; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS; y++) {
            qp_rect(my_display,
                    WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING), // left
                    WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING), // top
                    WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH, // right
                    WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT, // bottom
					WIDGET_MATRIX_COLOR_OFF,
                    false);
        }
    }
	// Render keymap
	widget_matrix_keymap_render();
}

void widget_matrix_keymap_render(void) {
	// uint16_t keycode[16];
	char buf1[4] = {0};
	
	// for (uint8_t i = 0;  i < MATRIX_ROWS; i++) {
		// for (uint8_t j = 0;  j < MATRIX_COLS; j++) {
			// keycode[i*4+j] = dynamic_keymap_get_keycode(0, i, j);
		// }
	// }
	
	uint8_t x_offset = WIDGET_MATRIX_POSX + WIDGET_MATRIX_KEY_WIDTH/2;
	uint8_t y_offset = WIDGET_MATRIX_POSY + WIDGET_MATRIX_KEY_HEIGHT/2;
	
	for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
		for (uint8_t j = 0;  j < MATRIX_COLS; j++) {
			// printf("%d - ", keycode[i*4+j]); // debug print
			// sprintf(buf1, "%s", keycode[i*4+j] + 61); // "A" is 0x04, convert to ASCII character by +61
			sprintf(buf1, "%s", keycode_to_string(dynamic_keymap_get_keycode(0, i, j)));
			qp_drawtext(my_display,
						j* (WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + x_offset - roboto20->line_height/4,
						i* (WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + y_offset - roboto20->line_height/2,
						roboto20,
						buf1);
		}
	}
}

void widget_matrix_update(uint8_t col, uint8_t row) {
	#ifdef SWAP_HANDS_ENABLE
	if (col != 5) { // TODO: hotfix for knob keypress
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
	#endif /* SWAP_HANDS_ENABLE */
}

char *keycode_to_string(enum qk_keycode_defines kc)
{
    switch (kc) {
#define X(keycode, hex, str) case keycode: return str;
        KEYCODE_LIST;
#undef X
    default:
        return "UNKNOWN";
    }
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
