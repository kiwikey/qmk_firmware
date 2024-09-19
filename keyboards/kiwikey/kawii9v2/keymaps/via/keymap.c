// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#if defined(OLED_ENABLE)
#include "encoder.h"
#include "oled/oled_menu.h"
#include "oled/oled_ui.h"
#endif // defined(OLED_ENABLE)

enum layer_names {
    _DEFAULT,
    _MEDIA,
    _MOUSE,
    _USER1,
    _USER2,
    _RGB,
};

enum tap_dance_kc {
    _TD_MENU
};

enum custom_keycodes {
	MACRO_WINCUT = QK_KB_0,
	MACRO_WINCOPY,
	MACRO_WINPASTE,
	MACRO_WINSHOWDSKTOP,
	MACRO_WINSNIP,
	MACRO_MACCUT,
	MACRO_MACCOPY,
	MACRO_MACPASTE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_DEFAULT] = LAYOUT(
        KC_A,      KC_B,      KC_C,
        KC_1,      KC_2,      KC_3,
        MO(_RGB),  KC_LCTL,   KC_LGUI,
		TD(_TD_MENU),   XXXXXXX
    ),
    [_MEDIA] = LAYOUT(
        KC_MSEL,   KC_MPRV,   KC_MNXT,    // Launch Player - Prev Track - Next Track
        KC_MPLY,   XXXXXXX,   XXXXXXX,    // Play/Pause
        MO(_RGB),  XXXXXXX,   XXXXXXX,
        TD(_TD_MENU),   XXXXXXX
    ),
    [_MOUSE] = LAYOUT(
        KC_BTN1,   KC_MS_U,   KC_BTN2,
        KC_MS_L,   KC_BTN1,   KC_MS_R,
        MO(_RGB),  KC_MS_D,   XXXXXXX,
        TD(_TD_MENU),   XXXXXXX
    ),
    [_USER1] = LAYOUT(
        XXXXXXX,   XXXXXXX,   XXXXXXX,
        XXXXXXX,   XXXXXXX,   XXXXXXX,
        MO(_RGB),  XXXXXXX,   XXXXXXX,
        TD(_TD_MENU),   XXXXXXX
    ),
    [_USER2] = LAYOUT(
        XXXXXXX,   XXXXXXX,   XXXXXXX,
        XXXXXXX,   XXXXXXX,   XXXXXXX,
        MO(_RGB),  XXXXXXX,   XXXXXXX,
        TD(_TD_MENU),   XXXXXXX
    ),
    [_RGB] = LAYOUT(
        RGB_TOG,   RGB_RMOD,  RGB_MOD,    // RGB On/Off - Next mode - Prev mode
        XXXXXXX,   RGB_HUD,   RGB_HUI,    //              Change "base" color
        XXXXXXX,   RGB_VAD,   RGB_VAI,    //              Change brightness
        TD(_TD_MENU),   XXXXXXX
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	if (record->event.pressed) {
		switch (keycode) {
			/* Windows */
			case MACRO_WINCUT:
				SEND_STRING(SS_LCTL("x"));
				break;
			case MACRO_WINCOPY:
				SEND_STRING(SS_LCTL("c"));
				break;
			case MACRO_WINPASTE:
				SEND_STRING(SS_LCTL("v"));
				break;
			case MACRO_WINSHOWDSKTOP:
				SEND_STRING(SS_LWIN("d"));
				break;
			case MACRO_WINSNIP:
				SEND_STRING(SS_LWIN(SS_LSFT("s")));
				break;
			/* Mac */
			case MACRO_MACCUT:
				SEND_STRING(SS_LCMD("x"));
				break;
			case MACRO_MACCOPY:
				SEND_STRING(SS_LCMD("c"));
				break;
			case MACRO_MACPASTE:
				SEND_STRING(SS_LCMD("v"));
				break;
		}
	}
	return true;
}

/*** TAP-DANCE THINGS ***/
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
