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

#include "rev1b.h"

#ifdef RGB_MATRIX_ENABLE
led_config_t g_led_config = {
  // Key Matrix to LED Index
  {
    {  0, 1, 2, 3 },
    {  4, 5, 6, 7 },
    {  8, 9,10,11 },
    { 12,13,14,15 }
  },
  // LED Index to Physical Position
  {
    /* RGB Matrix */
    {   0,   0 }, {  74,   0 }, { 149,   0 }, { 224,   0 }, 
    {   0,  21 }, {  74,  21 }, { 149,  21 }, { 224,  21 }, 
	{   0,  43 }, {  74,  43 }, { 149,  43 }, { 224,  43 }, 
	{   0,  64 }, {  74,  64 }, { 149,  64 }, { 224,  64 },
	/* RGB Underglow */
    {  37,  21 }, {  37,  64 }, { 187,  64 }, { 187,  21 }
  },
  // LED Index to Flag
  {
    /* RGB Matrix */
	4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 4,
	/* RGB Underglow */
    2, 2, 2, 2
  }
};
#endif