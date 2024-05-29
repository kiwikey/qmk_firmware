// Copyright 2022 KiwiKey (@KiwiKey)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "raw_hid.h"
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

void raw_hid_receive(uint8_t *data, uint8_t length) {
	// Create data array, size 64 bytes, and fill with 0x00 (to avoid weird data)
	uint8_t dataHIDhandshake[64];
	uint8_t dataHIDlayout[64];
	for (int i = 0; i < 64; i++) {
		dataHIDhandshake[i] = 0x00;
		dataHIDlayout[i] = 0x00;
	}
	
	// Define response package
	dataHIDhandshake[0] = 0xAA;
	for (int i = 1; i <= 9; i++) {
		dataHIDlayout[i] = i+5;
	}
	
	
	if (data[0] == 0x55) {
		raw_hid_send(dataHIDhandshake, length);		// If receive 0x55, confirm handshake
		rgblight_step();
	}
	if (data[0] == 0x69) {
		raw_hid_send(dataHIDlayout, length);		// If receive 0x69, send layout
		rgblight_step();
	}
	else {
		raw_hid_send(data, length);					// Or else, just echo what received
	}
}
