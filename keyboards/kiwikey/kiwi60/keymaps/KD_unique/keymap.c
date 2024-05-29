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
#include "eeprom.h"
//#include "ws2812/my_rgblight.h"

#define KD_LED_EEPROM_MODE	64	// position of LED status byte in EEPROM
#define KD_LED_EEPROM_PREVMODE	65	// position of LED status byte in EEPROM
#define KD_LED_EEPROM_COLOR	66	// position of LED color byte in EEPROM
#define KD_LED1_POS			16	// count from 0
#define KD_LED2_POS			17	// count from 0
#define KD_LED_NUM			3

uint8_t rgblight_mode_temp;
uint8_t KD_Color;

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BASE,
	_FN1,
    _FN2
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    KC_LED_TOG = SAFE_RANGE,
    KD_LED_MOD,
    KD_LED_RMOD,
    KD_LED_HUI,
    KD_LED_HUD
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [_BASE] = LAYOUT(
		KC_ESC,		KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINUS,    KC_EQUAL,    KC_DELETE,	KC_BSPACE,
		KC_TAB,		KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,     KC_RBRC,     KC_BSLASH, 
		KC_CAPS,	KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,     KC_ENTER,    KC_ENTER, 
		KC_LSHIFT,	KC_BSLS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_RSHIFT,   KC_UP,   	KC_SLASH,
		KC_LCTRL,	KC_LWIN, KC_LALT,						     KC_SPC,					KC_RALT, MO(_FN1),KC_LEFT,	   KC_DOWN,     KC_RIGHT
    ),
	[_FN1] = LAYOUT(
		KC_GRAVE,	KC_F1,	 KC_F2,	  KC_F3,   KC_F4, 	KC_F5,	 KC_F6,   KC_F7,   KC_F8, 	KC_F9, 	 KC_F10,  KC_F11, 	   KC_F12,	 	_______, 	KC_DELETE,
		RGB_TOG,	RGB_MOD, RGB_RMOD,RGB_M_P, RGB_M_B, RGB_M_R, _______, _______, _______, _______, KC_PSCR, _______,     _______,	    _______,
		_______,	RGB_HUI, RGB_HUD, _______, _______, _______, _______, _______, RESET,   _______, _______, _______, 	   _______,	    _______,
		MO(_FN2),	_______, RGB_VAI, RGB_VAD, _______, _______, _______, _______, _______, _______, _______, MO(_FN2),    _______,     _______,
		_______,	_______, _______,						     _______,					_______, _______, _______,	   _______,     _______
	),
	[_FN2] = LAYOUT(
		_______,	_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,     _______,	 	_______, 	_______,
		KC_LED_TOG,	KD_LED_MOD,_______,_______,  _______, _______, _______, _______, _______, _______, _______, _______,    _______,	    _______,
		_______,	KD_LED_HUI,_______,_______, _______, _______, _______, _______, _______, _______, _______, _______, 	_______,	    _______,
		_______,	_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, 	   _______,     _______,
		_______,	_______, _______,						     _______,					_______, _______, _______,	   _______,     _______
	)
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_LED_TOG:
            if (record->event.pressed) {
                rgblight_mode_temp = rgblight_get_mode();
				KD_Color = eeprom_read_byte((const uint8_t *)KD_LED_EEPROM_COLOR);
                rgblight_set_effect_range(KD_LED1_POS, KD_LED_NUM);
				if (eeprom_read_byte((const uint8_t *)KD_LED_EEPROM_MODE) == 0x00) {	// KD is currently off
					eeprom_write_byte((uint8_t *)KD_LED_EEPROM_MODE, eeprom_read_byte((const uint8_t *)KD_LED_EEPROM_PREVMODE)); // Turn it to previous mode
					rgblight_sethsv_range(KD_Color, 255, 255, KD_LED1_POS, KD_LED1_POS + KD_LED_NUM);
				} else {
					eeprom_write_byte((uint8_t *)KD_LED_EEPROM_PREVMODE, eeprom_read_byte((const uint8_t *)KD_LED_EEPROM_MODE));	// KD is currently on (no matter what mode)
					eeprom_write_byte((uint8_t *)KD_LED_EEPROM_MODE, 0x00);			// KD is currently on (no matter what mode)
					rgblight_sethsv_range(0, 0, 0, KD_LED1_POS, KD_LED1_POS + KD_LED_NUM);	// Turn it off
				}
                rgblight_set_effect_range(0, KD_LED1_POS);
                rgblight_mode(rgblight_mode_temp);
            } else {
            }
            break;        
        case KD_LED_MOD:
            if (record->event.pressed) {
				if (eeprom_read_byte((const uint8_t *)KD_LED_EEPROM_MODE) == 0x11) {	// KD is currently in stable color
					eeprom_write_byte((uint8_t *)KD_LED_EEPROM_MODE, 0x22);			// KD change to run with underglow
				} else {
					if (eeprom_read_byte((const uint8_t *)KD_LED_EEPROM_MODE) == 0x22) {		// KD is currently run with underglow
						eeprom_write_byte((uint8_t *)KD_LED_EEPROM_MODE, 0x11);				// KD change to stable color
					}
				}
				if (eeprom_read_byte((const uint8_t *)KD_LED_EEPROM_MODE) == 0x11) {	// KD LEDs stay 1 color
					rgblight_mode_temp = rgblight_get_mode();
					rgblight_set_effect_range(KD_LED1_POS, KD_LED_NUM);
					rgblight_sethsv_range(KD_Color, 255, 255, KD_LED1_POS, KD_LED1_POS + KD_LED_NUM);
					rgblight_set_effect_range(0, KD_LED1_POS);
					rgblight_mode(rgblight_mode_temp);
				}
				if (eeprom_read_byte((const uint8_t *)KD_LED_EEPROM_MODE) == 0x22) {	// KD LEDs run same effect with underglow LEDs
					rgblight_set_effect_range(0, KD_LED1_POS + KD_LED_NUM);
				}
            } else {
            }
            break;
        case KD_LED_RMOD:	// temporary not used
            if (record->event.pressed) {
 
            } else {
            }
            break;
        case KD_LED_HUI:
            if (record->event.pressed) {
				rgblight_mode_temp = rgblight_get_mode();
				if (KD_Color > 220)
					KD_Color = 0;
				else
					KD_Color += 21;
				eeprom_write_byte((uint8_t *)KD_LED_EEPROM_COLOR, KD_Color);
				rgblight_sethsv_range(KD_Color, 255, 255, KD_LED1_POS, KD_LED1_POS + KD_LED_NUM);
				rgblight_set_effect_range(0, KD_LED1_POS);
				rgblight_mode(rgblight_mode_temp);
            } else {
            }
            break;
    }
    return true;
}

void keyboard_post_init_user(void) {
	rgblight_set_effect_range(KD_LED1_POS, KD_LED_NUM);
	KD_Color = eeprom_read_byte((const uint8_t *)KD_LED_EEPROM_COLOR);
	switch (eeprom_read_byte((const uint8_t *)KD_LED_EEPROM_MODE)) {
		case 0x00:	// KD LED off
			rgblight_sethsv_range(0, 0, 0, KD_LED1_POS, KD_LED1_POS + KD_LED_NUM);
			rgblight_set_effect_range(0, KD_LED1_POS);
			break;
		case 0x11:	// KD LED color 1
			rgblight_sethsv_range(KD_Color, 255, 255, KD_LED1_POS, KD_LED1_POS + KD_LED_NUM);
			rgblight_set_effect_range(0, KD_LED1_POS);
			break;
		case 0x22:	// KD LED color 2
			rgblight_set_effect_range(0, KD_LED1_POS + KD_LED_NUM);
			break;
	}
}

/*
void matrix_init_user(void) {
}

void matrix_scan_user(void) {
}

bool led_update_user(led_t led_state) {
    return true;
}
*/
