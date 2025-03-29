// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#if defined(QUANTUM_PAINTER_ENABLE)
    #include "qp_graphics.h"	
	#include "qp/qp_includes.h"
	#include "qp/qp_menu.h"
	painter_device_t my_display;
#endif // defined(QUANTUM_PAINTER_ENABLE)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_4x12(
		KC_ESC,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
		KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_ENT,
		KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_UP,   KC_RSFT ,
		KC_LCTL, KC_LGUI, KC_LALT, KC_APP,  MO(1),   KC_SPC, KC_MUTE,     RGB_TOG, KC_SPC,  MO(2),   KC_RALT, KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [1] = LAYOUT_ortho_4x12(
		_______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_DEL,
		_______, _______, _______, _______, _______, _______,                      _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______,                      _______, _______, _______, _______, _______, _______,
		QK_BOOT, _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, _______, _______
    ),
    [2] = LAYOUT_ortho_4x12(
		_______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______,
		_______, _______, _______, _______, _______, _______,                      _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______,                      _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, _______, QK_BOOT
    ),
    [3] = LAYOUT_ortho_4x12(
		_______, _______, _______, _______, _______, _______,                      _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______,                      _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______,                      _______, _______, _______, _______, _______, _______,
		_______, _______, _______, _______, _______, _______, _______,    _______, _______, _______, _______, _______, _______, _______
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW( KC_VOLD, KC_VOLU ), ENCODER_CCW_CW( KC_PGUP, KC_PGDN ) },
    [1] = { ENCODER_CCW_CW( RGB_VAD, RGB_VAI ), ENCODER_CCW_CW( RGB_MOD, RGB_RMOD) },
    [2] = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) },
    [3] = { ENCODER_CCW_CW( _______, _______ ), ENCODER_CCW_CW( _______, _______ ) }
};
#endif

// void keyboard_post_init_user(void) {
    // void keyboard_post_init_display(void);
    // keyboard_post_init_display();
// }

void housekeeping_task_user(void) {
    // Update kb_state so we can send to slave
    // theme_state_update();

    // Data sync from master to slave
    // theme_state_sync();
}
