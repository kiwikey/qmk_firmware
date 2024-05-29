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

#define LAYOUT(\
K0_0,   K0_1,   K0_2,   K0_3,     K0_4,   K0_5,   K0_6,   K0_7,   K0_8,   K0_9,   K0_A,           K0_C,   K0_D,   K0_E,   K0_F,   K0_G,   K0_H,   K0_I,   K0_J,   K0_K,    \
K1_0,   K1_1,   K1_2,   K1_3,     K1_4,   K1_5,   K1_6,   K1_7,   K1_8,   K1_9,   K1_A,   K1_B,   K1_C,   K1_D,   K1_E,   K1_F,   K1_G,   K1_H,   K1_I,   K1_J,   K1_K,    \
K2_0,   K2_1,   K2_2,   K2_3,     K2_4,   K2_5,   K2_6,   K2_7,   K2_8,   K2_9,           K2_B,   K2_C,   K2_D,   K2_E,   K2_F,   K2_G,   K2_H,   K2_I,   K2_J,   K2_K,    \
K3_0,   K3_1,   K3_2,   K3_3,     K3_4,   K3_5,   K3_6,   K3_7,   K3_8,   K3_9,           K3_B,   K3_C,   K3_D,   K3_E,   K3_F,   K3_G,   K3_H,   K3_I,   K3_J,   K3_K,    \
K4_0,   K4_1,   K4_2,   K4_3,     K4_4,   K4_5,   K4_6,   K4_7,   K4_8,   K4_9,   K4_A,           K4_C,   K4_D,   K4_E,   K4_F,   K4_G,   K4_H,   K4_I,   K4_J,            \
K5_0,   K5_1,   K5_2,   K5_3,     K5_4,   K5_5,   K5_6,   K5_7,   K5_8,                                   K5_D,   K5_E,   K5_F,   K5_G,   K5_H,   K5_I,   K5_J,   K5_K   ) \
/* matrix positions */\
{\
{K0_0,   K0_1,   K0_2,   K0_3,   K0_4,   K0_5,   K0_6,   K0_7,   K0_8,   K0_9,   K0_A},\
{K1_0,   K1_1,   K1_2,   K1_3,   K1_4,   K1_5,   K1_6,   K1_7,   K1_8,   K1_9,   K1_A},\
{K2_0,   K2_1,   K2_2,   K2_3,   K2_4,   K2_5,   K2_6,   K2_7,   K2_8,   K2_9,   KC_NO},\
{K3_0,   K3_1,   K3_2,   K3_3,   K3_4,   K3_5,   K3_6,   K3_7,   K3_8,   K3_9,   KC_NO},\
{K4_0,   K4_1,   K4_2,   K4_3,   K4_4,   K4_5,   K4_6,   K4_7,   K4_8,   K4_9,   K4_A},\
{K5_0,   K5_1,   K5_2,   K5_3,   K5_4,   K5_5,   K5_6,   K5_7,   K5_8,   KC_NO,  KC_NO},\
\
{KC_NO,  K0_C,   K0_D,   K0_E,   K0_F,   K0_G,   K0_H,   K0_I,   K0_J,   K0_K,   KC_NO},\
{K1_B,   K1_C,   K1_D,   K1_E,   K1_F,   K1_G,   K1_H,   K1_I,   K1_J,   K1_K,   KC_NO},\
{K2_B,   K2_C,   K2_D,   K2_E,   K2_F,   K2_G,   K2_H,   K2_I,   K2_J,   K2_K,   KC_NO},\
{K3_B,   K3_C,   K3_D,   K3_E,   K3_F,   K3_G,   K3_H,   K3_I,   K3_J,   K3_K,   KC_NO},\
{KC_NO,  K4_C,   K4_D,   K4_E,   K4_F,   K4_G,   K4_H,   K4_I,   K4_J,   KC_NO,  KC_NO},\
{KC_NO,  KC_NO,  K5_D,   K5_E,   K5_F,   K5_G,   K5_H,   K5_I,   K5_J,   K5_K,   KC_NO} \
}
