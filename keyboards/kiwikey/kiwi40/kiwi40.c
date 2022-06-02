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

#include "kiwi40.h"

#ifdef RGB_MATRIX_ENABLE
led_config_t g_led_config = { {
  // Key Matrix to LED Index
    {  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 },
    {  24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34     },
    {  35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46 },
	{  47, 48, 49,     50,     51,     54, 55, 56, 57, 58 }
}, {
  // LED Index to Physical Position
    { 4, 10},  { 4, 51},  { 4, 92},  { 4,132},  { 4,173},  { 4, 214},  { 4,214},   { 4, 173},  { 4, 132},  { 4,  92},  { 4,  51},  { 4,  10}, 
	{ 0,  0},  { 0, 20},  { 0, 41},  { 0, 61},  { 0, 81},  { 0, 102},  { 0, 122},  { 0, 143},  { 0, 163},  { 0, 183},  { 0, 204},  { 0, 224}, 
	{22,  0},  {22, 20},  {22, 41},  {22, 61},  {22, 81},  {22, 102},  {22, 122},  {22, 143},  {22, 163},  {22, 183},  {22, 204},
	{43,  0},  {43, 20},  {43, 41},  {43, 61},  {43, 81},  {43, 102},  {43, 122},  {43, 143},  {43, 163},  {43, 183},  {43, 204},  {43, 224}, 
	{64,  0},  {64, 20},  {64, 41},  {64, 61},  {64, 81},  {64, 102},  {64, 122},  {64, 143},  {64, 163},  {64, 183},  {64, 204},  {64, 224},  {64, 224}
}, {
  // LED Index to Flag
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
} };
#endif

void matrix_init_kb(void) {
    rgb_matrix_enable();
	//rgb_matrix_mode(RGB_MATRIX_BREATHING);
}