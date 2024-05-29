// Copyright 2024 KiwiKey (@KiwiKey)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_MUTE,          KC_PSCR, KC_SCRL, KC_PAUS,
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_GRV,  KC_INS,  KC_HOME, KC_PGUP,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_DEL,  KC_END,  KC_PGDN,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT, KC_BSLS, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, MO(1),                     KC_UP,
        KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, KC_APP,  MO(1),   KC_RCTL,          KC_LEFT, KC_DOWN, KC_RIGHT
    ),
    [1] = LAYOUT(
        RGB_TOG, _______, _______, _______, KC_F14,  KC_F15,  KC_F16,  _______, _______, _______, _______, _______, _______, _______,          _______,  _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,  _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,   
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, MO(2),   _______,                    _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______,          _______,  _______, _______
    ),
    [2] = LAYOUT(
        QK_BOOT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,  _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,  _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,   
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                    _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______,          _______,  _______, _______
    ),
    [3] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,  _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,  _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,   
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,                    _______,
        _______, _______, _______,                            _______,                            _______, _______, _______, _______,          _______,  _______, _______
    )
};

typedef union {
    uint32_t raw;
    struct {
		//bool eeprom_reset;
        uint8_t rgblight_mode;
    };
} user_config_t;
user_config_t user_config;

// Mode 1 - F14: only underglow RGB (16 LEDs) - DEFAULT MODE!!!
// Mode 2 - F15: all (24 LEDs)
// Mode 3 - F16: only top RGB (8 LEDs)
#define KEYCODE_MODE1 KC_F14
#define KEYCODE_MODE2 KC_F15
#define KEYCODE_MODE3 KC_F16
void rgb_setmode_1(void) {
	rgblight_setrgb_range(0,0,0, 16, 24); // turn off all other LEDs (#16 to #23)
	rgblight_set_effect_range(0,16);
}
void rgb_setmode_2(void) {
	rgblight_set_effect_range(0,24);
}
void rgb_setmode_3(void) {
	rgblight_setrgb_range(0,0,0, 0, 16); // turn off all underglow LEDs (#00 to #15)
	rgblight_set_effect_range(16,8);
}

void eeconfig_init_user(void) {  // EEPROM is getting reset!
  user_config.raw = 0;
  user_config.rgblight_mode = 1; // We want this enabled by default
  eeconfig_update_user(user_config.raw); // Write default value to EEPROM now

  //use the non noeeprom versions, to write these values to EEPROM too
  // rgblight_enable(); // Enable RGB by default
  // rgblight_sethsv(HSV_CYAN);  // Set it to CYAN by default
  // rgblight_mode(1); // set to solid by default
}

void keyboard_post_init_user(void) {
  // Call the keymap level matrix init.

  // Read the user config from EEPROM
  user_config.raw = eeconfig_read_user();

  // Set default layer, if enabled
  if (user_config.rgblight_mode == 1) {
	rgb_setmode_1();
  }
  else if (user_config.rgblight_mode == 2) {
	rgb_setmode_2();
  }
  else if (user_config.rgblight_mode == 3) {
	rgb_setmode_3();
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KEYCODE_MODE1:
            if (record->event.pressed) {
				rgb_setmode_1();
				user_config.rgblight_mode = 1;
				eeconfig_update_user(user_config.raw);
            }
            break;
        case KEYCODE_MODE2:
            if (record->event.pressed) {
				rgb_setmode_2();
				user_config.rgblight_mode = 2;
				eeconfig_update_user(user_config.raw);
            }
            break;
        case KEYCODE_MODE3:
            if (record->event.pressed) {
				rgb_setmode_3();
				user_config.rgblight_mode = 3;
				eeconfig_update_user(user_config.raw);
            }
            break;
    }
    return true;
}
