#include QMK_KEYBOARD_H

#include "encoder.h"

#if defined(QUANTUM_PAINTER_ENABLE)
    #include "qp_graphics.h"	
	#include "qp/qp_includes.h"
	#include "qp/qp_menu.h"
	painter_device_t my_display;
#endif // defined(QUANTUM_PAINTER_ENABLE)

enum tap_dance_kc {
    _TD_MENU
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_5x5(
		KC_NUM,  KC_PSLS,   KC_PAST,  KC_PMNS, MO(1),
		KC_7,    KC_8,      KC_9,     KC_PPLS, MO(2),
		KC_4,    KC_5,      KC_6,     KC_PENT, MO(3),
        KC_1,    KC_2,      KC_3,     KC_UP,   KC_MUTE,
        KC_0,    KC_KP_DOT, KC_LEFT,  KC_DOWN, KC_RIGHT,
		TD(_TD_MENU) // Encoder
    ),
    [1] = LAYOUT_ortho_5x5(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		TD(_TD_MENU) // Encoder
    ),
    [2] = LAYOUT_ortho_5x5(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		TD(_TD_MENU) // Encoder
    ),
    [3] = LAYOUT_ortho_5x5(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
		TD(_TD_MENU) // Encoder
    )
};


/*** TAP-DANCE THINGS ***/
#if defined(TAP_DANCE_ENABLE)
void on_each_tap_fn(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 2) {               // When tap-dance is triggered
        if (current_menu == NOT_IN_MENU) { // if in UI   -> go to MainMenu
            menu_init();
        } else {                           // if in Menu -> exit Menu & go to UI
            menu_exit();
        }
        state->finished = true; // skip on_dance_finished_fn()
    }
}

void on_dance_finished_fn(tap_dance_state_t *state, void *user_data) {
    process_encoder_tap();
}

tap_dance_action_t tap_dance_actions[] = {
    [_TD_MENU] = ACTION_TAP_DANCE_FN_ADVANCED(on_each_tap_fn, on_dance_finished_fn, NULL)
};
#endif // defined(TAP_DANCE_ENABLE)