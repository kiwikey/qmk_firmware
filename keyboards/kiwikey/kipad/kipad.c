// Copyright 2022 KiwiKey (@KiwiKey)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kipad.h"

led_config_t g_led_config = { {
  // Key Matrix to LED Index
  {  0,  1,  2,  3  },
  {  4,  5,  6,  7  },
  {  8,  9,  10, 11 },
  {  12, 13, 14, 15 }
}, {
  // LED Matrix
  {  54, 25 }, {  93, 25 }, {  132, 25 }, {  171, 25 },
  {  54, 37 }, {  93, 37 }, {  132, 37 }, {  171, 37 },
  {  54, 49 }, {  93, 49 }, {  132, 49 }, {  171, 49 },
  {  54, 61 }, {  93, 61 }, {  132, 61 }, {  171, 61 },
  // 2 LED Lines
  {  222, 25 }, {  222, 30 }, {  222, 35 }, {  222, 40 }, {  222, 45 }, {  222, 50 }, {  222, 55 }, {  222, 60 },
  {  2, 25 }, {  2, 30 }, {  2, 35 }, {  2, 40 }, {  2, 45 }, {  2, 50 }, {  2, 55 }, {  2, 60 },
  // Underglow
  {  222, 0 }, {  148, 0 }, {  74, 0 }, {  0, 0 }, {  222, 21 }, {  222, 42 },
  {  222, 63 }, {  148, 63 }, {  74, 63 }, {  0, 63 }, {  0, 42 }, {  0, 21 }
}, {
  // LED Index to Flag
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  4, 4, 4, 4,
  8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
} };

void matrix_init_kb(void) {
    rgb_matrix_enable();
  //rgb_matrix_mode(RGB_MATRIX_BREATHING);
}

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { /* First encoder */
        if (clockwise) {
            tap_code_delay(KC_VOLU, 10);
        } else {
            tap_code_delay(KC_VOLD, 10);
        }
    } else if (index == 1) { /* Second encoder */
        if (clockwise) {
            tap_code_delay(KC_PGUP, 10);
        } else {
            tap_code_delay(KC_PGDN, 10);
        }
    }
    return false;
}
#endif
