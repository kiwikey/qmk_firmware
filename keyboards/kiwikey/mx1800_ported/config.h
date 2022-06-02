/*
Copyright 2021 KiwiKey

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "config_common.h"

/* USB Device descriptor parameter */
//#define VENDOR_ID       0xFEED
//#define PRODUCT_ID      0x6050
//#define DEVICE_VER      0x0100
#define VENDOR_ID       0x20A0
#define PRODUCT_ID      0x422E
#define DEVICE_VER      0x0200
#define MANUFACTURER    Ported by KiwiKey
#define PRODUCT         MX1800

/* key matrix size */
#define MATRIX_ROWS 6
#define MATRIX_COLS 20

#define MATRIX_COL_PINS { A0, A1, A2, A3, A4, A5, A6, A7, B0, B1, B2, B3, B4, B5, B6, B7, D4, D5, D6, D7 }
#define MATRIX_ROW_PINS { C2, C3, C4, C5, C6, C7 }
#define UNUSED_PINS

#define DIODE_DIRECTION COL2ROW

#define BACKLIGHT_PIN D0
#define BACKLIGHT_LEVELS 3

/* I2C parameters */
//#define I2C_BITRATE_KHZ 400
//#define I2C_TARGET_ADDR 0xB0

//#define WS2812_ADDRESS 0xb0 // default: 0xb0
//#define WS2812_TIMEOUT 100 // default: 100
#define RGBLED_NUM 18
#define RGBLIGHT_ANIMATIONS

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5
