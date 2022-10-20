/* Copyright 2022 KiwiKey
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include QMK_KEYBOARD_H
//#include "oled.c"
#include "print.h"
#include <qp.h>

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [0] = LAYOUT(
    KC_Q,    KC_W,    KC_E,    KC_R,
    KC_A,    KC_S,    KC_D,    KC_F,
    KC_Z,    KC_X,    KC_C,    MO(1)
    ),
    [1] = LAYOUT(
    _______, _______, _______, QK_BOOT,
    _______, _______, _______, _______,
    _______, _______, _______, _______
    ),
    [2] = LAYOUT(
    _______, _______, _______, _______,
    _______, _______, _______, _______,
    _______, _______, _______, _______
    ),
    [3] = LAYOUT(
    _______, _______, _______, _______,
    _______, _______, _______, _______,
    _______, _______, _______, _______
    )
};

//static painter_device_t display;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    return true;
}

void housekeeping_task_user(void) {
    static bool was_sent = false;
    if (!was_sent && timer_read32() > 3000) {
        uprintf("...3s!\n");
        was_sent = true;
    }
}

void keyboard_post_init_user(void) {
    uprintf("Post init user");
    SEND_STRING("Post init user");
}
