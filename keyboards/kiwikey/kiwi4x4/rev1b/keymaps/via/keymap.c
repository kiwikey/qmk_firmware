/* Copyright 2021 KiwiKey
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

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BASE,
    _FN
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
	[_BASE] = LAYOUT_ortho_4x4(
        KC_7,    KC_8,    KC_9,    	KC_KP_SLASH,
        KC_4,    KC_5,    KC_6,    	KC_KP_ASTERISK,
		KC_1,    KC_2,    KC_3,    	KC_KP_MINUS,
		KC_0,    KC_KP_DOT,  KC_KP_ENTER,	LT(_FN, KC_KP_PLUS)
    ),
	[_FN] = LAYOUT_ortho_4x4(
        RGB_TOG,  RGB_MOD,  RGB_RMOD,  KC_TRNS,
        RGB_HUD,  RGB_HUI,  RGB_VAD,   RGB_VAI,
		RGB_M_P,  RGB_M_B,  RGB_M_R,  RGB_M_SW,
		RESET,    RGB_SPD,  RGB_SPI,   KC_TRNS
    ),
	LAYOUT_ortho_4x4(
        KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,
        KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,
		KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,
		KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS
    ),
	LAYOUT_ortho_4x4(
        KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,
        KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,
		KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,
		KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS
    )
};