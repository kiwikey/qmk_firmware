// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_names {
    _DEFAULT,
    _MEDIA,
    _MOUSE,
    _USER1,
    _USER2,
    _RGB,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_DEFAULT] = LAYOUT(
        KC_WHOM,   KC_MYCM,   KC_CALC,    // Web Browser - My Computer  - Calculator
        C(KC_X),   C(KC_C),   C(KC_V),    //     Cut     -    Copy      -    Paste
        MO(_RGB),  MO(_MEDIA),MO(_MOUSE),
        KC_MUTE,   _______
    ),
    [_MEDIA] = LAYOUT(
        KC_MSEL,   KC_MPRV,   KC_MNXT,    // Launch Player - Prev Track - Next Track
        KC_MPLY,   XXXXXXX,   XXXXXXX,    // Play/Pause
        MO(_RGB),  XXXXXXX,   XXXXXXX,
        KC_MUTE,   _______
    ),
    [_MOUSE] = LAYOUT(
        KC_BTN1,   KC_MS_U,   KC_BTN2,
        KC_MS_L,   KC_BTN1,   KC_MS_R,
        MO(_RGB),  KC_MS_D,   XXXXXXX,
        KC_MUTE,   _______
    ),
    [_USER1] = LAYOUT(
        XXXXXXX,   XXXXXXX,   XXXXXXX,
        XXXXXXX,   XXXXXXX,   XXXXXXX,
        MO(_RGB),  XXXXXXX,   XXXXXXX,
        KC_MUTE,   _______
    ),
    [_USER2] = LAYOUT(
        XXXXXXX,   XXXXXXX,   XXXXXXX,
        XXXXXXX,   XXXXXXX,   XXXXXXX,
        MO(_RGB),  XXXXXXX,   XXXXXXX,
        KC_MUTE,   _______
    ),
    [_RGB] = LAYOUT(
        RGB_TOG,          RGB_RMOD,  RGB_MOD,    // RGB On/Off - Next mode - Prev mode
        CUSTOM_KC_MENU,   RGB_HUD,   RGB_HUI,    //              Change "base" color
        XXXXXXX,          RGB_VAD,   RGB_VAI,    //              Change brightness
        KC_MUTE,   _______
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_DEFAULT] = { ENCODER_CCW_CW( KC_VOLD,  KC_VOLU ), ENCODER_CCW_CW( KC_PGUP,  KC_PGDN ) },
    [_RGB] =     { ENCODER_CCW_CW( RGB_VAD,  RGB_VAI ), ENCODER_CCW_CW( RGB_RMOD, RGB_MOD ) },
    [_MEDIA] =   { ENCODER_CCW_CW( _______,  _______ ), ENCODER_CCW_CW( _______,  _______ ) },
    [_MOUSE] =   { ENCODER_CCW_CW( KC_WH_U,  KC_WH_D ), ENCODER_CCW_CW( _______,  _______ ) },
    [_USER1] =   { ENCODER_CCW_CW( KC_VOLD,  KC_VOLU ), ENCODER_CCW_CW( KC_PGUP,  KC_PGDN ) },
    [_USER2] =   { ENCODER_CCW_CW( KC_VOLD,  KC_VOLU ), ENCODER_CCW_CW( KC_PGUP,  KC_PGDN ) }
};
#endif
