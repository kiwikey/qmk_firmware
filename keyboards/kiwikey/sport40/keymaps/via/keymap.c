#include QMK_KEYBOARD_H
#include "sport40_graphics.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [0] = LAYOUT(
		KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,
		KC_A,    KC_B,    KC_C,    KC_D,    KC_E,    KC_F,
		KC_G,    KC_H,    KC_I,    KC_J,    KC_K,    KC_L,
		KC_M,    KC_N,    KC_O,    KC_P,    KC_Q,    MO(1)
    ),
    [1] = LAYOUT(
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, QK_BOOT,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS		
    ),
    [2] = LAYOUT(
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [3] = LAYOUT(
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][1][2] = {
    [0] = { ENCODER_CCW_CW( KC_VOLD, KC_VOLU ) },
    [1] = { ENCODER_CCW_CW( KC_PGUP, KC_PGDN ) },
    [2] = { ENCODER_CCW_CW( RGB_VAD, RGB_VAI ) },
    [3] = { ENCODER_CCW_CW( KC_TRNS, KC_TRNS ) }
};
#endif

void keyboard_post_init_user(void) {
    void keyboard_post_init_display(void);
    keyboard_post_init_display();
}
