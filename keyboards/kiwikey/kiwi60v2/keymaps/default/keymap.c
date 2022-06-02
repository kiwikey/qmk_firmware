/* Copyright 2020 KiwiKey
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

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BASE,
    _FN1
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    QMKBEST = SAFE_RANGE,
    QMKURL
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
 /*                                                                            ┌─────────┐
 *                                                                             │  BSPC   │
 *            ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┼────┬────┤
 *            │ESC │ 1  │ 2  │ 3  │ 4  │ 5  │ 6  │ 7  │ 8  │ 9  │ 0  │ -  │ =  │DEL │BSPC│
 *            ├────┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴────┤     ┌───────┐
 *            │ TAB  │ Q  │ W  │ E  │ R  │ T  │ Y  │ U  │ I  │ O  │ P  │ [  │ ]  │   \   │     │   \   │
 *            ├──────┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴───────┤  ┌──┴─┐     │
 *            │CAPSLOCK│ A  │ S  │ D  │ F  │ G  │ H  │ J  │ K  │ L  │ ;  │ '  │  ENTER   │  │    │ENTER│
 * ┌──────────┼─────┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴───┬──────┼──┴────┴─────┤
 * │  SHIFT   │SHIFT│    │ Z  │ X  │ C  │ V  │ B  │ N  │ M  │ ,  │ .  │ /  │SHIFT │      │    SHIFT    │
 * └──────────┼─────┼────┴┬───┴─┬──┴────┴────┴────┴────┴────┴────┼────┴┬───┴─┬────┴┬─────┼─────────────┘
 *            │CTRL │ WIN │ ALT │             SPACE              │ ALT │FN(1)│ APP │CTRL │
 *            └─────┴─────┴─────┴────────────────────────────────┴─────┴─────┴─────┴─────┘
 */
    [_BASE] = LAYOUT_DEFAULT_60A(
        KC_ESC,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINUS, KC_EQUAL, KC_DELETE, KC_BSPACE,
        KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,  KC_RBRC,  KC_BSLASH,
        KC_CAPS,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,  KC_GRAVE, KC_ENTER,
        KC_LSHIFT,XXXXXXX, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLASH, XXXXXXX,  KC_RSHIFT, XXXXXXX,
        KC_LCTRL, KC_LWIN,          KC_LALT,                   KC_SPC,                             KC_RALT, MO(_FN1),           KC_APP,    KC_RCTRL
    ),
    [_FN1] = LAYOUT_DEFAULT_60A(
        RESET,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,
        KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,
        KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,
        KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS,   KC_TRNS,
        KC_TRNS,  KC_TRNS,          KC_TRNS,                   KC_TRNS,                            KC_TRNS, KC_TRNS,            KC_TRNS,   KC_TRNS
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return true;
}
