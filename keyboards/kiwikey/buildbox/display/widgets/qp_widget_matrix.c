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
	// Draw shadow
	qp_roundrect(WIDGET_MATRIX_POSX - WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW,
				 WIDGET_LAYER_POSY + UI_WIDGET_SHADOW - 5 + 1,
				//  WIDGET_MATRIX_POSY - WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW + 1 - WIDGET_LAYER_HEIGHT,
				 WIDGET_MATRIX_POSX + WIDGET_MATRIX_KEY_WIDTH*4  + WIDGET_MATRIX_KEY_SPACING*3 + WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW,
				 WIDGET_MATRIX_POSY + WIDGET_MATRIX_KEY_HEIGHT*4 + WIDGET_MATRIX_KEY_SPACING*3 + WIDGET_MATRIX_BORDER + UI_WIDGET_SHADOW,
				 UI_COLOR_SHADOW, true, 5, true, true);
	// Draw background
	qp_roundrect(WIDGET_MATRIX_POSX - WIDGET_MATRIX_BORDER,
				 WIDGET_LAYER_POSY - 5 + 1,
				 WIDGET_MATRIX_POSX + WIDGET_MATRIX_KEY_WIDTH*4  + WIDGET_MATRIX_KEY_SPACING*3 + WIDGET_MATRIX_BORDER,
				 WIDGET_MATRIX_POSY + WIDGET_MATRIX_KEY_HEIGHT*4 + WIDGET_MATRIX_KEY_SPACING*3 + WIDGET_MATRIX_BORDER,
				 WIDGET_MATRIX_BG, true, 5, true, true);
	// Draw all buttons
    for (uint8_t x = 0; x < MATRIX_ROWS-1; x++) { // ROW4 is for direct pin buttons, so need to -1
        for (uint8_t y = 0; y < MATRIX_COLS; y++) {
			widget_matrix_bgclear_singlebutton(x, y);
			widget_matrix_render_singlebutton(x, y, WIDGET_MATRIX_BUTTON_OFF, false, 0); // as long as text_on = false, layer is ignored
		}
	}
	// 2 FREE BUTTONS
	// Shadows
	qp_circle(my_display,
			  WIDGET_MATRIX_BTN1_POSX + UI_WIDGET_SHADOW, WIDGET_MATRIX_BTN1_POSY + UI_WIDGET_SHADOW,
			  WIDGET_MATRIX_BTN_RADIUS, UI_COLOR_SHADOW, true);
	qp_circle(my_display,
			  WIDGET_MATRIX_BTN2_POSX + UI_WIDGET_SHADOW, WIDGET_MATRIX_BTN2_POSY + UI_WIDGET_SHADOW,
			  WIDGET_MATRIX_BTN_RADIUS, UI_COLOR_SHADOW, true);
	// Inner
	qp_circle(my_display,
			  WIDGET_MATRIX_BTN1_POSX, WIDGET_MATRIX_BTN1_POSY,
			  WIDGET_MATRIX_BTN_RADIUS, WIDGET_MATRIX_BUTTON_BG, true);
	qp_circle(my_display,
			  WIDGET_MATRIX_BTN2_POSX, WIDGET_MATRIX_BTN2_POSY,
			  WIDGET_MATRIX_BTN_RADIUS, WIDGET_MATRIX_BUTTON_BG, true);
	// Outline
	qp_circle(my_display,
			  WIDGET_MATRIX_BTN1_POSX, WIDGET_MATRIX_BTN1_POSY,
			  WIDGET_MATRIX_BTN_RADIUS, WIDGET_MATRIX_BUTTON_OFF, false);
	qp_circle(my_display,
			  WIDGET_MATRIX_BTN2_POSX, WIDGET_MATRIX_BTN2_POSY,
			  WIDGET_MATRIX_BTN_RADIUS, WIDGET_MATRIX_BUTTON_OFF, false);
	// Button icons
	qp_drawimage(my_display,
				 WIDGET_MATRIX_BTN1_POSX - (ico16_layer->width)/2,
				 WIDGET_MATRIX_BTN1_POSY - (ico16_layer->height)/2,
				 ico16_layer);
	qp_drawimage(my_display,
				 WIDGET_MATRIX_BTN2_POSX - (ico32_menu->width)/2,
				 WIDGET_MATRIX_BTN2_POSY - (ico32_menu->height)/2,
				 ico32_menu);
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
				widget_matrix_render_singlebutton(x, y, WIDGET_MATRIX_KC_COLOR, true, layer);
		}
	}
}

/***  Render the keycode string for "basic keycodes" ***/
void widget_matrix_render_kc_basic(uint16_t posx, uint16_t posy, uint16_t keycode) {
	if (posx == NULL_VALUE) return; // with matrix positions that are "blank", their keycode will be 0x0000, same as KC_NO, so must not process them
	char buf1[5] = {0}; // maximum 4 characters + null terminator = 5 bytes
	sprintf(buf1, "%s", keycode_to_string(keycode));
	qp_drawtext_recolor_center(my_display, posx, posy,
							   WIDGET_MATRIX_KC_BASIC_FONT,
							   buf1, WIDGET_MATRIX_KC_COLOR, WIDGET_MATRIX_KC_BG);
}

/***  Render the keycode string for "layer keycodes"
	PLEASE NOTE : moving between layers means to be handled by free button, not the matrix key
				  this will run only when user sets a matrix key to layer keycode
	There is a yellow label in top-left corner of the key, with: "MO" / "LT" / "TG" / ...
***/
void widget_matrix_render_kc_layer(uint16_t posx, uint16_t posy, uint16_t keycode) {
	// Top-left label background, TODO: clean this up
	qp_rect(my_display,
			posx - WIDGET_MATRIX_KEY_WIDTH/2 + 1,
			posy - WIDGET_MATRIX_KEY_WIDTH/2 + 1,
			posx - WIDGET_MATRIX_KEY_WIDTH/2 + 16,
			posy - WIDGET_MATRIX_KEY_WIDTH/2 + WIDGET_MATRIX_LABEL_FONT->line_height + 2,
			WIDGET_MATRIX_LABEL_BG, true);
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
	qp_drawtext_recolor(my_display,
						posx - WIDGET_MATRIX_KEY_WIDTH/2 +3,
						posy - WIDGET_MATRIX_KEY_WIDTH/2 +3,
						WIDGET_MATRIX_LABEL_FONT, buf1, HSV_BLACK, WIDGET_MATRIX_LABEL_BG);
	// Layer number
	qp_drawtext_recolor_center(my_display,
							   posx,
							   posy + 5, // HOTFIX: +5 for better alignment
							   WIDGET_MATRIX_KC_BASIC_FONT,
							   buf2, WIDGET_MATRIX_KC_COLOR, WIDGET_MATRIX_KC_BG);
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
	qp_rect(my_display,
			WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING), // left
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING), // top
			WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH, // right
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT, // bottom
			hue, sat, val,
			false);
	// Inner text
	if (text_on) {
		widget_matrix_bgclear_singlebutton(x, y);
		uint16_t x_offset = WIDGET_MATRIX_POSX + y* (WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH/2;
		uint16_t y_offset = WIDGET_MATRIX_POSY + x* (WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT/2;
		uint16_t keycode = dynamic_keymap_get_keycode(layer, x, y);
		switch (keycode) {
			case QK_MOMENTARY ... QK_PERSISTENT_DEF_LAYER_MAX: // All layer-related keycodes (0x5220 to 0x52FF)
				widget_matrix_render_kc_layer(x_offset, y_offset, keycode);
				break;
			case BASIC_KEYCODE_RANGE:
			case MODIFIER_KEYCODE_RANGE:
			case USER_KEYCODE_RANGE:
				widget_matrix_render_kc_basic(x_offset, y_offset, keycode);
				break;
			default:
				widget_matrix_render_kc_basic(x_offset, y_offset, keycode);
				// ;
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
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + 1, // top
			WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH - 1, // right
			WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT - 1, // bottom
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
