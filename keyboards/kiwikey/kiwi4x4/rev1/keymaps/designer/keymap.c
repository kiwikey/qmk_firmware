/* Copyright 2020 LTT
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include "raw_hid.h"

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _PTS = 0,
	_AI  = 1,
    _FN
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    QMKBEST = SAFE_RANGE,
    QMKURL
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[_PTS] = LAYOUT(
        KC_B,    		KC_E,    		KC_W,    		KC_P,
        C(KC_J),    	C(KC_E),    	C(KC_M),    	C(KC_U),
		C(KC_PLUS),    	C(KC_MINUS),    KC_SPACE,    	C(A(S(KC_N))),
		C(KC_Z),    	C(KC_D),  		C(KC_ENTER),	MO(_FN)
    ),
	[_AI] = LAYOUT(
        KC_A,    		KC_V,    		KC_P,    		KC_T,
        C(KC_PLUS),    	C(KC_MINUS),    C(KC_LBRC),    	C(KC_RBRC),
		S(KC_E),    	S(KC_B),    	C(S(KC_LBRC)),   C(S(KC_RBRC)),
		C(KC_D),    	C(KC_Y),  		KC_C,			MO(_FN)
    ),
    [_FN] = LAYOUT(
        DF(_PTS),    		DF(_AI),    RGB_TOG,    		RGB_MODE_FORWARD,
        C(KC_N),			C(KC_O),             XXXXXXX,    XXXXXXX,
		XXXXXXX,			XXXXXXX,             XXXXXXX,    RESET,
        KC_VOLU,			KC_VOLD,    	     KC_MUTE,    XXXXXXX
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QMKBEST:
            if (record->event.pressed) {
                // when keycode QMKBEST is pressed
                SEND_STRING("QMK is the best thing ever!");
            } else {
                // when keycode QMKBEST is released
            }
            break;
        case QMKURL:
            if (record->event.pressed) {
                // when keycode QMKURL is pressed
                SEND_STRING("https://qmk.fm/\n");
            } else {
                // when keycode QMKURL is released
            }
            break;			
    }
    return true;
}

/*
led_config_t g_led_config = { {
	  // Key Matrix to LED Index
	  { 0, 1, 2, 3 },
	  { 4, 5, 6, 7 },
	  { 8, 9, 10, 11 },
	  { 12, 13, 14, 15 }
	}, {
	  // LED Index to Physical Position
	  {  0, 0 }, {  0, 75 }, {  0, 149 }, {  0, 224 },
	  { 21, 0 }, { 21, 75 }, { 21, 149 }, { 21, 224 },
	  { 42, 0 }, { 42, 75 }, { 42, 149 }, { 42, 224 },
	  { 64, 0 }, { 64, 75 }, { 64, 149 }, { 64, 224 }
	}, {
	  // LED Index to Flag
	  1, 1, 1, 1,
	  1, 1, 1, 1,
	  1, 1, 1, 1,
	  1, 1, 1, 1
	} };
	*/

void matrix_init_user(void) {
//	rgb_matrix_disable();
//	rgb_matrix_enable();
}

/*
void matrix_scan_user(void) {

}

bool led_update_user(led_t led_state) {
    return true;
}
*/
