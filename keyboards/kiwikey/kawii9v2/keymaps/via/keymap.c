#include QMK_KEYBOARD_H
#include "oled-bongocat.c"
#include "encoder.c"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_3x3(
        KC_P7,   KC_P8,   KC_P9,
        KC_P4,   KC_P5,   KC_P6,
        MO(1),   MO(2),   MO(3),
		KC_MUTE, KC_MUTE
    ),
	[1] = LAYOUT_ortho_3x3(
        KC_TRNS,   KC_TRNS,   KC_TRNS,
        KC_TRNS,   KC_TRNS,   KC_TRNS,
        KC_TRNS,   KC_TRNS,   QK_BOOT,
		KC_TRNS,   KC_TRNS
    ),
	[2] = LAYOUT_ortho_3x3(
        RGB_TOG,   RGB_RMOD,  RGB_MOD,
        KC_TRNS,   RGB_VAD,   RGB_VAI,
        KC_TRNS,   KC_TRNS,   KC_TRNS,
		RGB_TOG,   RGB_TOG
    ),
	[3] = LAYOUT_ortho_3x3(
        KC_TRNS,   KC_TRNS,   KC_TRNS,
        KC_TRNS,   KC_TRNS,   KC_TRNS,
        KC_TRNS,   KC_TRNS,   KC_TRNS,
		KC_TRNS,   KC_TRNS
    )
};
