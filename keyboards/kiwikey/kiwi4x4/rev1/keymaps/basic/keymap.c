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
    _BASE = 0,
    _FN
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    QMKBEST = SAFE_RANGE,
    QMKURL
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
	[_BASE] = LAYOUT(
        KC_7,    KC_8,    KC_9,    	KC_KP_SLASH,
        KC_4,    KC_5,    KC_6,    	KC_KP_ASTERISK,
		KC_1,    KC_2,    KC_3,    	KC_KP_MINUS,
		KC_0,    KC_KP_DOT,  KC_KP_ENTER,	LT(_FN, KC_KP_PLUS)
    ),
    [_FN] = LAYOUT(
        RGB_TOG,    		RGB_MODE_FORWARD,    KC_BRIU,    KC_BRID,
        LWIN(LSFT(KC_S)),	KC_WHOM,             RGB_VAI,    RGB_VAD,
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

void matrix_init_user(void) {
	rgb_matrix_disable();
	rgb_matrix_enable();
}

/*
void matrix_scan_user(void) {

}

bool led_update_user(led_t led_state) {
    return true;
}
*/

void raw_hid_receive(uint8_t *data, uint8_t length) {
	// Create data array, size 64 bytes, and fill with 0x00 (to avoid weird data)
	uint8_t dataHIDhandshake[64];
	uint8_t dataHIDlayout[64];
	for (int i = 0; i < 64; i++) {
		dataHIDhandshake[i] = 0x00;
		dataHIDlayout[i] = 0x00;
	}
	
	// Define response package
	dataHIDhandshake[0] = 0xAA;
	for (int i = 1; i <= 9; i++) {
		dataHIDlayout[i] = i+5;
	}
	
	
	if (data[0] == 0x55) {
		raw_hid_send(dataHIDhandshake, length);		// If receive 0x55, confirm handshake
		rgblight_step();
	}
	if (data[0] == 0x69) {
		raw_hid_send(dataHIDlayout, length);		// If receive 0x69, send layout
		rgblight_step();
	}
	else {
		raw_hid_send(data, length);					// Or else, just echo what received
	}
}