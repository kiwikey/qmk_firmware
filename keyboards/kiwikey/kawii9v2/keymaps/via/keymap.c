// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_names {
    _COMMON,
    _MEDIA,
    _NUMPAD,
	_MOUSE,
	_MACRO1,
	_MACRO2,
	_USER1,
	_USER2
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_COMMON] = LAYOUT(
        KC_WHOM,   KC_CALC,   KC_MYCM,    // Launch application:  Browser    - Calculator - My Computer
        KC_MPLY,   KC_MPRV,   KC_MNXT,    // Media track control: Play/Pause - Prev Track - Next Track
        MO(1),     LCTL(KC_C),LCTL(KC_V), //                      FN(1)      - Copy       - Paste
        KC_MUTE,   _______
    ),
    [_MEDIA] = LAYOUT(
        KC_MSEL,   RGB_RMOD,  RGB_MOD,    // Launch Player - 
        KC_MPLY,   KC_MPRV,   KC_MNXT,    // Play/Pause    - Prev Track - Next Track
        MO(1),     _______,   _______,
        KC_MUTE,   _______
    ),
    [_NUMPAD] = LAYOUT(
        _______,   _______,   _______,
        _______,   _______,   _______,
        _______,   _______,   MO(3),
        _______,   _______
    ),
    [_MOUSE] = LAYOUT(
        QK_BOOT,   KC_MS_U,    _______,
        KC_MS_L,   _______,   KC_MS_R,
        _______,   KC_MS_D,   _______,
        _______,   _______
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW( KC_VOLD,  KC_VOLU ), ENCODER_CCW_CW( KC_PGUP,  KC_PGDN ) },
    [1] = { ENCODER_CCW_CW( RGB_VAD,  RGB_VAI ), ENCODER_CCW_CW( RGB_RMOD, RGB_MOD ) },
    [2] = { ENCODER_CCW_CW( _______,  _______ ), ENCODER_CCW_CW( _______,  _______ ) },
    [3] = { ENCODER_CCW_CW( _______,  _______ ), ENCODER_CCW_CW( _______,  _______ ) }
};
#endif

    // [_OLD] = LAYOUT(
        // KC_WHOM,   KC_CALC,   KC_MYCM,    // Launch application:  Browser    - Calculator - My Computer
        // KC_MPLY,   KC_MPRV,   KC_MNXT,    // Media track control: Play/Pause - Prev Track - Next Track
        // MO(1),     LCTL(KC_C),LCTL(KC_V), //                      FN(1)      - Copy       - Paste
        // KC_MUTE,   _______
    // ),
	
	// [_RGB] = LAYOUT(
        // RGB_TOG,   RGB_RMOD,  RGB_MOD,
        // KC_MPLY,   KC_MPRV,   KC_MNXT,    // Media track control: Play/Pause - Prev Track - Next Track
        // MO(1),     _______,   _______,
        // KC_MUTE,   _______
    // ),