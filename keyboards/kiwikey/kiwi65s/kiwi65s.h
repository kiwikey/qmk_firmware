/* Copyright 2021 KiwiKey
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

#pragma once

#include "quantum.h"

/* This is a shortcut to help you visually see your layout.
 *
 * The first section contains all of the arguments representing the physical
 * layout of the board and position of the keys.
 *
 * The second converts the arguments into a two-dimensional array which
 * represents the switch matrix.
 */
#define LAYOUT( \
	L00,  L01,  L02,  L03,  L04,  L05,  L06,  L07,        R01,  R02,  R03,  R04,  R05,  R06,  R07,  R08,  \
	L10,  L11,  L12,  L13,  L14,  L15,  L16,        R10,  R11,  R12,  R13,  R14,  R15,  R16,  R17,  R18,  \
	L20,  L21,  L22,  L23,  L24,  L25,  L26,        R20,  R21,  R22,  R23,  R24,  R25,        R27,  R28,  \
	L30,  L31,        L33,  L34,  L35,  L36,  L37,        R31,  R32,  R33,  R34,  R35,  R36,  R37,  R38,  \
	L40,  L41,  L42,  L43,  L44,        L46,                    R42,  R43,  R44,  R45,  R46,  R47,  R48   \
    ) \
    { \
		{ L00,   L01,   L02,   L03,   L04,   L05,   L06,   L07,   KC_NO },  \
		{ L10,   L11,   L12,   L13,   L14,   L15,   L16,   KC_NO, KC_NO },  \
		{ L20,   L21,   L22,   L23,   L24,   L25,   L26,   KC_NO, KC_NO },  \
		{ L30,   L31,   KC_NO, L33,   L34,   L35,   L36,   L37,   KC_NO },  \
		{ L40,   L41,   L42,   L43,   L44,   KC_NO, L46,   KC_NO, KC_NO },  \
		{ R08,   R07,   R06,   R05,   R04,   R03,   R02,   R01,   KC_NO },  \
		{ R18,   R17,   R16,   R15,   R14,   R13,   R12,   R11,   R10   },  \
		{ R28,   R27,   KC_NO, R25,   R24,   R23,   R22,   R21,   R20   },  \
		{ R38,   R37,   R36,   R35,   R34,   R33,   R32,   R31,   KC_NO },  \
		{ R48,   R47,   R46,   R45,   R44,   R43,   R42,   KC_NO, KC_NO }   \
    }
