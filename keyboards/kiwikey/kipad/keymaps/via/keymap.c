// Copyright 2022 KiwiKey (@KiwiKey)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "oled-bongocat.c"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT(
    KC_0,    KC_1,    KC_2,    KC_3,
    KC_4,    KC_5,    KC_6,    KC_7,
    KC_A,    KC_B,    KC_C,    KC_D,
    KC_E,    KC_F,    KC_G,    MO(1)
  ),
  [1] = LAYOUT(
    RGB_TOG, RGB_RMOD,RGB_MOD, QK_BOOT,
	_______, _______, _______, _______,
	_______, _______, _______, _______,
	_______, _______, _______, _______
  ),
  [2] = LAYOUT(
    _______, _______, _______, _______,
	_______, _______, _______, _______,
	_______, _______, _______, _______,
	_______, _______, _______, _______
  ),
  [3] = LAYOUT(
    _______, _______, _______, _______,
	_______, _______, _______, _______,
	_______, _______, _______, _______,
	_______, _______, _______, _______
  )
};
