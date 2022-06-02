/*
This is the c configuration file for the keymap

Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

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

//#define USE_MATRIX_I2C

/* Select hand configuration */
#define MASTER_LEFT
// #define MASTER_RIGHT
// #define EE_HANDS

#define RGBLIGHT_SPLIT

#define USE_SERIAL_PD2

#define TAPPING_FORCE_HOLD
#define TAPPING_TERM 100

#define RGB_MAX_BRIGHTNESSS 128
#ifdef RGBLIGHT_ENABLE
    #undef RGBLED_NUM
//    #define RGBLIGHT_ANIMATIONS
    #define RGBLED_NUM 40
    #define RGBLIGHT_LIMIT_VAL RGB_MAX_BRIGHTNESSS
    #define RGBLIGHT_HUE_STEP 10
    #define RGBLIGHT_SAT_STEP 17
    #define RGBLIGHT_VAL_STEP 17
#endif

#define OLED_FONT_H "keyboards/crkbd/lib/glcdfont.c"

#ifdef RGB_MATRIX_ENABLE
#   define RGB_MATRIX_KEYPRESSES // reacts to keypresses
// #   define RGB_MATRIX_KEYRELEASES // reacts to keyreleases (instead of keypresses)
// #   define RGB_DISABLE_AFTER_TIMEOUT 0 // number of ticks to wait until disabling effects
#   define RGB_DISABLE_WHEN_USB_SUSPENDED true // turn off effects when suspended
#   define RGB_MATRIX_FRAMEBUFFER_EFFECTS
// #   define RGB_MATRIX_LED_PROCESS_LIMIT (DRIVER_LED_TOTAL + 4) / 5 // limits the number of LEDs to process in an animation per task run (increases keyboard responsiveness)
// #   define RGB_MATRIX_LED_FLUSH_LIMIT 16 // limits in milliseconds how frequently an animation will update the LEDs. 16 (16ms) is equivalent to limiting to 60fps (increases keyboard responsiveness)
#    define RGB_MATRIX_MAXIMUM_BRIGHTNESS RGB_MAX_BRIGHTNESSS
#    define RGB_MATRIX_HUE_STEP 8
#    define RGB_MATRIX_SAT_STEP 8
#    define RGB_MATRIX_VAL_STEP 8
#    define RGB_MATRIX_SPD_STEP 10

/* Disable the animations you don't want/need.  You will need to disable a good number of these    *
 * because they take up a lot of space.  Disable until you can successfully compile your firmware. */
// #define DISABLE_RGB_MATRIX_SOLID_COLOR				// OK
 #define DISABLE_RGB_MATRIX_ALPHAS_MODS
 #define DISABLE_RGB_MATRIX_GRADIENT_UP_DOWN
 #define DISABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
// #define DISABLE_RGB_MATRIX_BREATHING					// OK
 #define DISABLE_RGB_MATRIX_BAND_SAT
 #define DISABLE_RGB_MATRIX_BAND_VAL
 #define DISABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
 #define DISABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
 #define DISABLE_RGB_MATRIX_BAND_SPIRAL_SAT
 #define DISABLE_RGB_MATRIX_BAND_SPIRAL_VAL
// #   define DISABLE_RGB_MATRIX_CYCLE_ALL				// OK
// #   define DISABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT			// OK
// #   define DISABLE_RGB_MATRIX_CYCLE_UP_DOWN			// OK
// #   define DISABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON	// OK
// #   define DISABLE_RGB_MATRIX_CYCLE_OUT_IN				// OK
// #   define DISABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL		// OK
// #define DISABLE_RGB_MATRIX_CYCLE_PINWHEEL				// OK
// #define DISABLE_RGB_MATRIX_CYCLE_SPIRAL				// OK
// #   define DISABLE_RGB_MATRIX_DUAL_BEACON				// OK
// #   define DISABLE_RGB_MATRIX_RAINBOW_BEACON			// OK
// #   define DISABLE_RGB_MATRIX_RAINBOW_PINWHEELS		// OK
 #   define DISABLE_RGB_MATRIX_RAINDROPS
 #define DISABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
 #define DISABLE_RGB_MATRIX_HUE_BREATHING
 #define DISABLE_RGB_MATRIX_HUE_PENDULUM				// OK but out of sync, need to fix
// #define DISABLE_RGB_MATRIX_HUE_WAVE					// OK
// #   define DISABLE_RGB_MATRIX_TYPING_HEATMAP			// OK
 #   define DISABLE_RGB_MATRIX_DIGITAL_RAIN				// Not OK, do not enable
// #   define DISABLE_RGB_MATRIX_SOLID_REACTIVE			// OK
// #   define DISABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE	// OK
 #   define DISABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE		// Error
 #   define DISABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE	// Error
 #   define DISABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS		// Error
 #   define DISABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS// Error
 #   define DISABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS		// Error
 #   define DISABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS// Error
 #   define DISABLE_RGB_MATRIX_SPLASH					// Not nice effect
 #   define DISABLE_RGB_MATRIX_MULTISPLASH				// Not nice effect
 #   define DISABLE_RGB_MATRIX_SOLID_SPLASH				// Not nice effect
// #   define DISABLE_RGB_MATRIX_SOLID_MULTISPLASH		// OK
#endif