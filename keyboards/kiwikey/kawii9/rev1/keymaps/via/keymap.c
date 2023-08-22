// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_MUTE,    KC_VOLD,    KC_VOLU,    // Media volume: Mute - Volume Up - Volume Down
        KC_MPLY,    KC_MPRV,    KC_MNXT,    // Media track control: Play/Pause - Previous Track - Next Track
        MO(1),      LCTL(KC_C), LCTL(KC_V)  // FN(1) - Copy - Paste
    ),
    [1] = LAYOUT(
        RGB_TOG,    RGB_RMOD,   RGB_MOD,
        _______,    RGB_M_B,    RGB_M_R,
        _______,    MO(2),      _______
    ),
    [2] = LAYOUT(
        _______,    _______,    _______,
        _______,    _______,    _______,
        _______,    _______,    MO(3)
    ),
    [3] = LAYOUT(
        QK_BOOT,    _______,    _______,
        _______,    _______,    _______,
        _______,    _______,    _______
    )
};
