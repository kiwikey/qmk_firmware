#include QMK_KEYBOARD_H
#include "oled.c"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_3x3(
        KC_P7,   KC_P8,   KC_P9,
        KC_P4,   KC_P5,   KC_P6,
        MO(1),   KC_P2,   KC_P3,
		KC_MUTE, KC_WHOM
    ),
	[1] = LAYOUT_ortho_3x3(
        _______,   _______,   _______,
        _______,   _______,   _______,
        _______,   MO(2),     _______,
		_______,   _______
    ),
	[2] = LAYOUT_ortho_3x3(
        RGB_TOG,   RGB_RMOD,  RGB_MOD,
        _______,   RGB_VAD,   RGB_VAI,
        _______,   _______,   MO(3),
		_______,   _______
    ),
	[3] = LAYOUT_ortho_3x3(
        QK_BOOT,   _______,   _______,
        _______,   _______,   _______,
        _______,   _______,   _______,
		_______,   _______
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][2][2] = {
    [0] = { ENCODER_CCW_CW( KC_VOLD,  KC_VOLU ), ENCODER_CCW_CW( KC_PGUP,  KC_PGDN ) },
    [1] = { ENCODER_CCW_CW( RGB_VAD,  RGB_VAI ), ENCODER_CCW_CW( RGB_RMOD, RGB_MOD ) },
    [2] = { ENCODER_CCW_CW( _______,  _______ ), ENCODER_CCW_CW( _______,  _______ ) },
    [3] = { ENCODER_CCW_CW( _______,  _______ ), ENCODER_CCW_CW( _______,  _______ ) }
};
#endif
