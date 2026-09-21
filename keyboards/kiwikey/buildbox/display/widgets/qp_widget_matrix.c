#if defined(QUANTUM_PAINTER_ENABLE)

#include "quantum.h"
#include "qp_widget_matrix.h"

#include "features/eeprom_custom.h"
#include "features/keycodes_list.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"

/***  Render all initialization things for the Matrix Widget:
	+ widget shadow
	+ widget background
	+ widget buttons (4x4 = 16)
	+ widget free buttons (2 buttons)
***/
void widget_matrix_init(void) {
    for (uint8_t x = 0; x < MATRIX_ROWS-1; x++) { // ROW4 is for direct pin buttons, so need to -1
        for (uint8_t y = 0; y < MATRIX_COLS; y++) {
			widget_matrix_bgclear_singlebutton(x, y);
			widget_matrix_render_singlebutton(x, y, WIDGET_MATRIX_BUTTON_OFF, false, 0); // as long as text_on = false, layer is ignored
		}
	}
}

/***  Render "interactive effect" when a key is being pressed
	The current effect: outline of key (a square) change color from WIDGET_MATRIX_BUTTON_OFF to WIDGET_MATRIX_BUTTON_ON
	This is called during:
		+ process_record_display()
***/
void widget_matrix_update(uint8_t col, uint8_t row) {
		bool on = (matrix_get_row(row) & (1 << col)) > 0; // The matrix position [x,y] is being pressed or released
		if (row != 4) { // Not direct pin buttons
			if (on)
				widget_matrix_render_singlebutton(row, col, WIDGET_MATRIX_BUTTON_ON, false, 0); // pressed
			else
				widget_matrix_render_singlebutton(row, col, WIDGET_MATRIX_BUTTON_OFF, false, 0); // released
		}
		// else if (row == 4) // TODO
}

/***  Render keycode string of the whole matrix (4x4)
	This is called during:
		+ ui_refresh()
		+ layer changes (layer_state_set_kb)
***/
void widget_matrix_keymap_render(uint8_t layer) {
	for (uint8_t x = 0; x < MATRIX_ROWS-1; x++) {
		for (uint8_t y = 0;  y < MATRIX_COLS; y++) {
				widget_matrix_render_singlebutton(x, y, WIDGET_MATRIX_BUTTON_OFF, true, layer);
		}
	}
}

/***  Render 'line_count' pre-formatted strings, stacked and centered as one block
	around (posx, posy) - the shared layout primitive behind every kc_* renderer below
***/
static void widget_matrix_render_kc_lines(uint16_t posx, uint16_t posy, const char * const lines[], uint8_t line_count) {
	uint16_t line_h = WIDGET_MATRIX_KC_BASIC_FONT->line_height;
	uint16_t top    = posy - (line_count * line_h) / 2;
	for (uint8_t i = 0; i < line_count; i++) {
		qp_drawtext_recolor_center(my_display,
									posx,
									top + i*line_h + line_h/2,
									WIDGET_MATRIX_KC_BASIC_FONT,
									lines[i],
									WIDGET_MATRIX_KC_COLOR,
									WIDGET_MATRIX_KC_BG);
	}
}

/***  Render the keycode string for "basic keycodes" ***/
void widget_matrix_render_kc_basic(uint16_t posx, uint16_t posy, uint16_t keycode) {
	if (keycode == NULL_VALUE) return; // with matrix positions that are "blank", their keycode will be 0x0000, same as KC_NO, so must not process them
	char buf1[5] = {0}; // maximum 4 characters + null terminator = 5 bytes
	sprintf(buf1, "%s", keycode_to_string(keycode));
	const char *lines[1] = { buf1 };
	widget_matrix_render_kc_lines(posx, posy, lines, 1);
}

/***  Render macro keycodes (QK_MACRO_0..31) as 2 lines: "MACRO" + its number ***/
void widget_matrix_render_kc_macro(uint16_t posx, uint16_t posy, uint16_t keycode) {
	char buf1[3]; // macro number is 0-31, max 2 digits + null terminator
	snprintf(buf1, sizeof(buf1), "%d", (keycode - QK_MACRO_0) & 0x1F); // & 0x1F proves the 0-31 bound to the compiler (QK_MACRO_0..31 is 5 bits)
	const char *lines[2] = { "MACRO", buf1 };
	widget_matrix_render_kc_lines(posx, posy, lines, 2);
}

/***  3-letter label for a Mod-Tap's "hold" modifier; chorded combos fall back to "MOD" ***/
static const char *widget_matrix_mod_tap_label(uint8_t mods) {
	switch (mods & 0x0F) { // low nibble only - bit 4 (L/R flag) doesn't change the label
		case MOD_LCTL: return "CTL";
		case MOD_LSFT: return "SFT";
		case MOD_LALT: return "ALT";
		case MOD_LGUI: return "GUI";
		default:       return "MOD"; // multiple mods held together, or none
	}
}

/***  Render hold-tap keycodes (Mod-Tap / Layer-Tap) as 2 lines:
	"hold" function on top (mod label, or L<n> for a layer), "tap" function (a basic
	keycode, reusing keycode_to_string()) on the bottom
***/
void widget_matrix_render_kc_holdtap(uint16_t posx, uint16_t posy, uint16_t keycode) {
	char        buf1[4]; // "L" + layer number (0-15) + null terminator
	const char *hold_label;
	uint8_t     tap_kc;
	if (IS_QK_LAYER_TAP(keycode)) {
		snprintf(buf1, sizeof(buf1), "L%d", QK_LAYER_TAP_GET_LAYER(keycode));
		hold_label = buf1;
		tap_kc     = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
	} else { // Mod-Tap
		hold_label = widget_matrix_mod_tap_label(QK_MOD_TAP_GET_MODS(keycode));
		tap_kc     = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
	}
	const char *lines[2] = { hold_label, keycode_to_string(tap_kc) };
	widget_matrix_render_kc_lines(posx, posy, lines, 2);
}

/***  Render one single key to the screen, at [x,y] position, with HSV color
	If filled = TRUE, then draw the whole key with:
				+ background = WIDGET_MATRIX_BUTTON_BG
				+ outline = HSV
				+ keycode
	if not, draw the outline only
***/
void widget_matrix_render_singlebutton(uint8_t x, uint8_t y, uint8_t hue, uint8_t sat, uint8_t val, bool text_on, uint8_t layer) {
	// Button outline
	qp_line(my_display,
			WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) +3, // left
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) +2, // top
			WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH -3, // right
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) +2, // top
			hue, sat, val);
	qp_line(my_display,
			WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) +3, // left
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT -2, // bottom
			WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH -3, // right
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT -2, // bottom
			hue, sat, val);
	// Inner text
	if (text_on) {
		widget_matrix_bgclear_singlebutton(x, y);
		uint16_t x_offset = WIDGET_MATRIX_POSX + y* (WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH/2;
		uint16_t y_offset = WIDGET_MATRIX_POSY + x* (WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT/2;
		uint16_t keycode = dynamic_keymap_get_keycode(layer, x, y);
		switch (keycode) {
			case BASIC_KEYCODE_RANGE:
			case MODIFIER_KEYCODE_RANGE:
			case USER_KEYCODE_RANGE:
				widget_matrix_render_kc_basic(x_offset, y_offset, keycode);
				break;
			case MACRO_KEYCODE_RANGE:
				widget_matrix_render_kc_macro(x_offset, y_offset, keycode);
				break;
			case QK_MOD_TAP ... QK_LAYER_TAP_MAX: // Mod-Tap (0x2000-0x3FFF) and Layer-Tap (0x4000-0x4FFF) are contiguous
				widget_matrix_render_kc_holdtap(x_offset, y_offset, keycode);
				break;
			default:
				widget_matrix_render_kc_basic(x_offset, y_offset, keycode);
		}
	}
}

/***  Clear the background of the whole matrix (4x4)
	Nothing from the old keycode is left
***/
// void widget_matrix_bgclear(void) {
//     for (uint8_t x = 0; x < MATRIX_ROWS-1; x++) { // ROW4 is for direct pin buttons, so need to -1
//         for (uint8_t y = 0; y < MATRIX_COLS; y++) {
// 			widget_matrix_bgclear_singlebutton(x, y);
// 		}
// 	}
// }

/***  Clear the background of a single key to WIDGET_MATRIX_BUTTON_BG
	Nothing from the old keycode is left
***/
void widget_matrix_bgclear_singlebutton(uint8_t x, uint8_t y) { // just inner button, has no effect to button's outline
	qp_rect(my_display,
			WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + 1, // left
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + 3, // top
			WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH - 1, // right
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT - 3, // bottom
			WIDGET_MATRIX_BUTTON_BG,
			true);
}

/***  Custom function for converting keycode (eg. 0x0004) to text (eg. "a")
	Keycode strings are get from 'keycodes_list.h'
	If not defined, return "!?"
***/
char *keycode_to_string(enum qk_keycode_defines kc) {
    switch (kc) {
#define X(keycode, hex, str) case keycode: return str;
        KEYCODE_LIST;
#undef X
    default:
        return "!?";
    }
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
