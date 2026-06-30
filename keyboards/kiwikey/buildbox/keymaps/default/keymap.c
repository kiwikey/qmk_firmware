// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "i2c_master.h"
#include "magnetic_encoder.h"
#include <lib/lib8tion/lib8tion.h>

extern painter_device_t my_display;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_numpad_4x4(
        KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
        KC_P4,   KC_P5,   KC_P6,   KC_A,
        KC_P1,   KC_P2,   KC_P3,   KC_PENT,
        KC_P0,   KC_C,    KC_PDOT, KC_D
    )
};

void keyboard_post_init_user(void) {
	// debug_enable=true;
	// debug_matrix=true;
	// debug_keyboard=true;
	// debug_mouse=true;
}

void housekeeping_task_user(void) {
	// print("print ok");
	// uprintf("uprintf ok");
	// dprint("dprint ok");
	// dprintf("dprintf ok");
}
