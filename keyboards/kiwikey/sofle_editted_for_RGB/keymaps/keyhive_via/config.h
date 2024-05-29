 /* Copyright 2020 Josef Adamcik
  * Modification for VIA support and RGB underglow by Jens Bonk-Wiltfang
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

/* The way how "handedness" is decided (which half is which),
see https://docs.qmk.fm/#/feature_split_keyboard?id=setting-handedness
for more options.
*/

#define EE_HANDS

//Add RGB underglow
#define RGB_DI_PIN D3
#define RGBLED_NUM 80
#define DRIVER_LED_TOTAL RGBLED_NUM		// added
#define RGBLED_SPLIT {40,40}
//#define RGBLIGHT_ANIMATIONS

#define RGB_MAX_BRIGHTNESSS 128
#define RGBLIGHT_LIMIT_VAL RGB_MAX_BRIGHTNESSS
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS RGB_MAX_BRIGHTNESSS