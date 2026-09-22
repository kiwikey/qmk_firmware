// Minimal keymap for the RP2040<->CH582F bridge bring-up target.
//
// QMK defaults to CONNECTION_HOST_AUTO, which prefers USB over Bluetooth
// whenever USB is physically connected (see quantum/connection/connection.c
// connection_auto_detect_host()) — and USB has to stay connected here for
// power. So the last key is QK_OUTPUT_BLUETOOTH, not KC_E: press it once
// to force output to Bluetooth (persists in EEPROM), THEN use A/B/C/D to
// actually exercise the bridge/BLE path.
#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(KC_A, KC_B, KC_C, QK_OUTPUT_USB, QK_OUTPUT_BLUETOOTH),
};
