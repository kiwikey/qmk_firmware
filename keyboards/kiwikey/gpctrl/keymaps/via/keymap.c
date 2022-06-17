// Copyright 2022 KiwiKey (@KiwiKey)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT(
    KC_0,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,   KC_7,   KC_8,    TG(1),
    KC_A,    KC_B,    KC_C,    KC_D,    KC_E,    KC_F,    KC_G,   KC_H,   KC_I
  ),
  [1] = LAYOUT(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
  )
};
