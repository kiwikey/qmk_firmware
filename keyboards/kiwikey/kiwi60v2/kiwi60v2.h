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

#pragma once

#include "quantum.h"

#define LAYOUT_FULL( \
     k00,   k01,   k02,   k03,   k04,   k05,   k06,   k07,   k08,   k09,   k0A,   k0B,   k0C,   k0D,   k0E, \
     k10,   k11,   k12,   k13,   k14,   k15,   k16,   k17,   k18,   k19,   k1A,   k1B,   k1C,   k1D,        \
     k20,   k21,   k22,   k23,   k24,   k25,   k26,   k27,   k28,   k29,   k2A,   k2B,   k2C,   k2D,        \
     k30,   k31,   k32,   k33,   k34,   k35,   k36,   k37,   k38,   k39,   k3A,   k3B,   k3C,   k3D,   k3E, \
     k40,   k41,          k43,   k44,   k45,   k46,   k47,   k48,   k49,   k4A,   k4B,   k4C,   k4D,   k4E  \
) \
{ \
    {k00,   k01,   k02,   k03,   k04,   k05,   k06,   k07,   k08,   k09,   k0A,   k0B,   k0C,   k0D,   k0E   },   \
    {k10,   k11,   k12,   k13,   k14,   k15,   k16,   k17,   k18,   k19,   k1A,   k1B,   k1C,   k1D,   KC_NO },   \
    {k20,   k21,   k22,   k23,   k24,   k25,   k26,   k27,   k28,   k29,   k2A,   k2B,   k2C,   k2D,   KC_NO },   \
    {k30,   k31,   k32,   k33,   k34,   k35,   k36,   k37,   k38,   k39,   k3A,   k3B,   k3C,   k3D,   k3E   },   \
    {k40,   k41,   KC_NO, k43,   k44,   k45,   k46,   k47,   k48,   k49,   k4A,   k4B,   k4C,   k4D,   k4E   }    \
}

/* Default 60% layout (A)
 *             ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬─────────┐    ┌────┬────┐
 *             │ 00 │ 01 │ 02 │ 03 │ 04 │ 05 │ 06 │ 07 │ 08 │ 09 │ 0A │ 0B │ 0C │   0E    │    │ 0D │ 0E │
 *             ├────┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬───────┤    └────┴────┘       ┌───────┐
 *             │  10  │ 11 │ 12 │ 13 │ 14 │ 15 │ 16 │ 17 │ 18 │ 19 │ 1A │ 1B │ 1C │  1D   │                      │       │
 *             ├──────┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴───────┤   ┌────┬─────┐       └─┐ 2D  │
 *             │   20   │ 21 │ 22 │ 23 │ 24 │ 25 │ 26 │ 27 │ 28 │ 29 │ 2A │ 2B │    2D    │   │ 2C │ 2D  │         │     │
 * ┌────┬─────┐├────────┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴──────────┤┌──┴───┬┴─────┤┌────┬───┴┬────┤
 * │ 30 │ 31  ││    30    │ 32 │ 33 │ 34 │ 35 │ 36 │ 37 │ 38 │ 39 │ 3A │ 3B │     3D      ││  3D  │  3E  ││ 3C │ 3D │ 3E │
 * └────┴─────┘├─────┬────┴┬───┴─┬──┴────┴────┴────┴────┴────┴────┼────┴┬───┴─┬─────┬─────┤└──────┴──────┘└────┴────┴────┘
 *             │ 40  │ 41  │ 43  │              46                │ 4A  │ 4B  │ 4D  │ 4E  │
 *             └─────┴─────┴─────┴────────────────────────────────┴─────┴─────┴─────┴─────┘
 */
#define LAYOUT_DEFAULT_60A( \
     k00,   k01,   k02,   k03,   k04,   k05,   k06,   k07,   k08,   k09,   k0A,   k0B,   k0C,   k0D,   k0E, \
     k10,   k11,   k12,   k13,   k14,   k15,   k16,   k17,   k18,   k19,   k1A,   k1B,   k1C,   k1D,        \
     k20,   k21,   k22,   k23,   k24,   k25,   k26,   k27,   k28,   k29,   k2A,   k2B,   k2C,   k2D,        \
     k30,   k31,   k32,   k33,   k34,   k35,   k36,   k37,   k38,   k39,   k3A,   k3B,   k3C,   k3D,   k3E, \
     k40,   k41,          k43,                 k46,                        k4A,   k4B,          k4D,   k4E  \
) \
{ \
    {k00,   k01,   k02,   k03,   k04,   k05,   k06,   k07,   k08,   k09,   k0A,   k0B,   k0C,   k0D,   k0E   },   \
    {k10,   k11,   k12,   k13,   k14,   k15,   k16,   k17,   k18,   k19,   k1A,   k1B,   k1C,   k1D,   KC_NO },   \
    {k20,   k21,   k22,   k23,   k24,   k25,   k26,   k27,   k28,   k29,   k2A,   k2B,   k2C,   k2D,   KC_NO },   \
    {k30,   k31,   k32,   k33,   k34,   k35,   k36,   k37,   k38,   k39,   k3A,   k3B,   k3C,   k3D,   k3E   },   \
    {k40,   k41,   KC_NO, k43,   KC_NO, KC_NO, k46,   KC_NO, KC_NO, KC_NO, k4A,   k4B,   KC_NO, k4D,   k4E   }    \
}

/* Default 60% layout (B)
 *             ┌────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬─────────┐    ┌────┬────┐
 *             │ 00 │ 01 │ 02 │ 03 │ 04 │ 05 │ 06 │ 07 │ 08 │ 09 │ 0A │ 0B │ 0C │   0E    │    │ 0D │ 0E │
 *             ├────┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬───────┤    └────┴────┘       ┌───────┐
 *             │  10  │ 11 │ 12 │ 13 │ 14 │ 15 │ 16 │ 17 │ 18 │ 19 │ 1A │ 1B │ 1C │  1D   │                      │       │
 *             ├──────┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴───────┤   ┌────┬─────┐       └─┐ 2D  │
 *             │   20   │ 21 │ 22 │ 23 │ 24 │ 25 │ 26 │ 27 │ 28 │ 29 │ 2A │ 2B │    2D    │   │ 2C │ 2D  │         │     │
 * ┌────┬─────┐├────────┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴─┬──┴──────────┤┌──┴───┬┴─────┤┌────┬───┴┬────┤
 * │ 30 │ 31  ││    30    │ 32 │ 33 │ 34 │ 35 │ 36 │ 37 │ 38 │ 39 │ 3A │ 3B │     3D      ││  3D  │  3E  ││ 3C │ 3D │ 3E │
 * └────┴─────┘├─────┬────┴┬───┴─┬──┴────┴────┴────┴────┴────┴───┬┴───┬┴───┬┴───┬────┬────┤└──────┴──────┘└────┴────┴────┘
 *             │ 40  │ 41  │ 43  │              46               │ 4A │ 4B │ 4C │ 4D │ 4E │
 *             └─────┴─────┴─────┴───────────────────────────────┴────┴────┴────┴────┴────┘
 */
#define LAYOUT_DEFAULT_60B( \
     k00,   k01,   k02,   k03,   k04,   k05,   k06,   k07,   k08,   k09,   k0A,   k0B,   k0C,   k0D,   k0E, \
     k10,   k11,   k12,   k13,   k14,   k15,   k16,   k17,   k18,   k19,   k1A,   k1B,   k1C,   k1D,        \
     k20,   k21,   k22,   k23,   k24,   k25,   k26,   k27,   k28,   k29,   k2A,   k2B,   k2C,   k2D,        \
     k30,   k31,   k32,   k33,   k34,   k35,   k36,   k37,   k38,   k39,   k3A,   k3B,   k3C,   k3D,   k3E, \
     k40,   k41,          k43,                 k46,                        k4A,   k4B,   k4C,   k4D,   k4E  \
) \
{ \
    {k00,   k01,   k02,   k03,   k04,   k05,   k06,   k07,   k08,   k09,   k0A,   k0B,   k0C,   k0D,   k0E   },   \
    {k10,   k11,   k12,   k13,   k14,   k15,   k16,   k17,   k18,   k19,   k1A,   k1B,   k1C,   k1D,   KC_NO },   \
    {k20,   k21,   k22,   k23,   k24,   k25,   k26,   k27,   k28,   k29,   k2A,   k2B,   k2C,   k2D,   KC_NO },   \
    {k30,   k31,   k32,   k33,   k34,   k35,   k36,   k37,   k38,   k39,   k3A,   k3B,   k3C,   k3D,   k3E   },   \
    {k40,   k41,   KC_NO, k43,   KC_NO, KC_NO, k46,   KC_NO, KC_NO, KC_NO, k4A,   k4B,   k4C,   k4D,   k4E   }    \
}
