#if defined(QUANTUM_PAINTER_ENABLE)

#include "quantum.h"
#include "qp_widget_matrix.h"

#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"
#include "display/eeprom_custom.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"
#include "keycodes_list.h"

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
    for (uint8_t x = 0; x < MATRIX_ROWS-1; x++) { // ROW4 is for direct pin buttons, so need to -1
        for (uint8_t y = 0; y < MATRIX_COLS; y++) {
			widget_matrix_render_button(x, y, WIDGET_MATRIX_COLOR_OFF, false);
		}
	}
	// Render keymap
	// widget_matrix_keymap_render(0); // No need, handled in keyboard_post_init_display()
}

void widget_matrix_update(uint8_t col, uint8_t row) {
		bool on = (matrix_get_row(row) & (1 << col)) > 0; // The matrix position [x,y] is being pressed
		if (row != 4) { // Not direct pin buttons
			if (on)
				widget_matrix_render_button(row, col, WIDGET_MATRIX_COLOR_ON, false);
			else
				widget_matrix_render_button(row, col, WIDGET_MATRIX_COLOR_OFF, false);
		}
}

void widget_matrix_keymap_render(uint8_t layer) {
	for (uint8_t x = 0; x < MATRIX_ROWS-1; x++) {
		for (uint8_t y = 0;  y < MATRIX_COLS; y++) {
			uint16_t keycode = dynamic_keymap_get_keycode(layer, x, y);
			uint16_t x_offset = WIDGET_MATRIX_POSX + y* (WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING);
			uint16_t y_offset = WIDGET_MATRIX_POSY + x* (WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING);
			// Draw keycode string
			switch (keycode) {
				case QK_MOMENTARY ... QK_PERSISTENT_DEF_LAYER_MAX: // All layer-related keycodes (0x5220 to 0x52FF)
					widget_matrix_render_kc_layer(x_offset, y_offset, keycode);
					break;
				case BASIC_KEYCODE_RANGE:
				case MODIFIER_KEYCODE_RANGE:
					widget_matrix_render_kc_basic(x_offset, y_offset, keycode);
					// sprintf(buf1, "%s", keycode_to_string(keycode));
					break;
				default:
					;// TODO
			}
		}
	}
}

void widget_matrix_render_button(uint8_t x, uint8_t y, uint8_t hue, uint8_t sat, uint8_t val, bool filled) { // x and y are matrix [x,y], not pixel-related
	// Button background
	// qp_rect(my_display,
	// 		WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING), // left
	// 		WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING), // top
	// 		WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH, // right
	// 		WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT, // bottom
	// 		WIDGET_MATRIX_BG,
	// 		true);
	// Button outline
	qp_rect(my_display,
			WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING), // left
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING), // top
			WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH, // right
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT, // bottom
			hue, sat, val,
			filled);
}

void widget_matrix_render_kc_basic(uint8_t posx, uint8_t posy, uint16_t keycode) {
	char buf1[4] = {0};
	sprintf(buf1, "%s", keycode_to_string(keycode));
	qp_drawtext_recolor_center( my_display,
								posx + WIDGET_MATRIX_KEY_WIDTH/2,
								posy + WIDGET_MATRIX_KEY_HEIGHT/2,
								WIDGET_MATRIX_KC_BASIC_FONT,
								buf1, WIDGET_MATRIX_KC_BASIC_COLOR, WIDGET_MATRIX_KC_BASIC_BG);
}

void widget_matrix_render_kc_layer(uint8_t posx, uint8_t posy, uint16_t keycode) {
	// Top-left label background
	qp_rect(my_display,
			posx + 1,
			posy + 1,
			posx + 16,
			posy + WIDGET_MATRIX_LABEL_FONT->line_height + 2,
			WIDGET_MATRIX_LABEL_LAYER_BG, true);
	// Type of layer-keycode
	char buf1[4], buf2[10]; // TODO: optimize buf1 buf2 length
	switch (keycode) {
		case QK_MOMENTARY ... QK_MOMENTARY_MAX:
			sprintf(buf1, "MO");
			sprintf(buf2, "%i", keycode - QK_MOMENTARY);
			break;
		case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX:
			sprintf(buf1, "TG");
			sprintf(buf2, "%i", keycode - QK_TOGGLE_LAYER);
			break;
		default:
			sprintf(buf1, "??");
			sprintf(buf2, "??");
	}
	// Top-left label
	qp_drawtext_recolor(my_display, posx+3, posy+3, WIDGET_MATRIX_LABEL_FONT, buf1, HSV_BLACK, WIDGET_MATRIX_LABEL_LAYER_BG);
	qp_drawtext_recolor_center( my_display,
								posx + WIDGET_MATRIX_KEY_WIDTH/2,
								posy + WIDGET_MATRIX_KEY_HEIGHT/2 + 5, // +5 for better alignment
								WIDGET_MATRIX_KC_BASIC_FONT,
								buf2, WIDGET_MATRIX_KC_BASIC_COLOR, WIDGET_MATRIX_KC_BASIC_BG);
}

char *keycode_to_string(enum qk_keycode_defines kc)
{
    switch (kc) {
#define X(keycode, hex, str) case keycode: return str;
        KEYCODE_LIST;
#undef X
    default:
        return "!?";
    }
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
