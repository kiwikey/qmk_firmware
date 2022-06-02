/* Copyright 2020 TTL
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
#include <stdio.h>

char wpm_str[10];

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BASE,
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
		KC_ESC,		KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINUS,    KC_EQUAL,    KC_DELETE,	KC_BSPACE,
		KC_TAB,		KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,     KC_RBRC,     KC_BSLASH, 
		KC_CAPS,	KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,     KC_ENTER,    KC_ENTER, 
		KC_LSHIFT,	KC_BSLS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_RSHIFT,   KC_UP,   	KC_SLASH,
		KC_LCTRL,	KC_LWIN, KC_LALT,						     KC_SPC,					MO(_FN), KC_RCTRL,KC_LEFT,	   KC_DOWN,     KC_RIGHT
    ),
	[_FN] = LAYOUT(
		KC_GRAVE,	KC_F1,	 KC_F2,	  KC_F3,   KC_F4, 	KC_F5,	 KC_F6,   KC_F7,   KC_F8, 	KC_F9, 	 KC_F10,  KC_F11, 	   KC_F12,	 	_______, 	KC_DELETE,
		RGB_TOG,	RGB_MOD, RGB_RMOD,RGB_M_P, RGB_M_B, RGB_M_R, _______, _______, _______, _______, KC_PSCR, _______,     _______,	    _______,
		_______,	RGB_HUI, RGB_HUD, _______, _______, _______, _______, _______, RESET,   _______, _______, _______, 	   _______,	    _______,
		_______,	_______, RGB_VAI, RGB_VAD, _______, _______, _______, _______, _______, _______, _______, _______, 	   _______,	    _______,
		_______,	_______, _______,						     _______,					_______, _______, _______,	   _______,     _______
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
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
	return OLED_ROTATION_90;
}

#ifdef OLED_DRIVER_ENABLE
void oled_task_user(void) {
    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer"), false);

    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_P(PSTR("BASE\n"), false);
            break;
        case _FN:
            oled_write_P(PSTR("FN\n"), false);
            break;
        default:
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_ln_P(PSTR("Undefined\n"), false);
    }
	
	oled_set_cursor(0, 5);
	sprintf(wpm_str, "WPM: %03d", get_current_wpm());
	oled_write(wpm_str, false);

    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
	oled_set_cursor(0, 14);
    oled_write_P(led_state.num_lock ? PSTR("NUM ") : PSTR("    "), false);
	oled_set_cursor(0, 15);
    oled_write_P(led_state.caps_lock ? PSTR("CAP ") : PSTR("    "), false);
//    oled_write_P(led_state.scroll_lock ? PSTR("SCR ") : PSTR("    "), false);
}
#endif
*/

/*
void matrix_init_user(void) {

}

void matrix_scan_user(void) {

}

bool led_update_user(led_t led_state) {
    return true;
}
*/
