// Default keymap for the Sofle left half over the CH582F BLE bridge.
//
// QMK defaults to CONNECTION_HOST_AUTO, which prefers USB over Bluetooth
// whenever USB is physically connected (see quantum/connection/connection.c
// connection_auto_detect_host()) — and USB may stay connected here for
// power. The key labeled BT in keyboard.json's layout (matrix [4,2]) is
// QK_OUTPUT_BLUETOOTH: press it once to force output to Bluetooth, which
// persists across reboots (EEPROM).
#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_GRV,  KC_1,    KC_2,    KC_3, KC_4,    KC_5,
        KC_ESC,  KC_Q,    KC_W,    KC_E, KC_R,    KC_T,
        KC_TAB,  KC_A,    KC_S,    KC_D, KC_F,    KC_G,
        KC_LSFT, KC_Z,    KC_X,    KC_C, KC_V,    KC_B,
        KC_LCTL, KC_LGUI, QK_OUTPUT_BLUETOOTH, KC_LALT, KC_ENT
    ),
};

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
    return false;
}
