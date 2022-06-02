/* Copyright 2020 TTL
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
#include <print.h>
#include <stdio.h>

#define dot_ms	50
#define timeout 300
#define dot		1
#define dash	0

#define alphabet_len 36
const uint8_t alphabet[alphabet_len][5] = {
	{ 1,0,3,3,3 },	// A
	{ 0,1,1,1,3 },	// B
	{ 0,1,0,1,3 },	// C
	{ 0,1,1,3,3 },	// D
	{ 1,3,3,3,3 },	// E
	{ 1,1,0,1,3 },	// F
	{ 0,0,1,3,3 },	// G
	{ 1,1,1,1,3 },	// H
	{ 1,1,3,3,3 },	// I
	{ 1,0,0,0,3 },	// J
	{ 0,1,0,3,3 },	// K
	{ 1,0,1,1,3 },	// L
	{ 0,0,3,3,3 },	// M
	{ 0,1,3,3,3 },	// N
	{ 0,0,0,3,3 },	// O
	{ 1,0,0,1,3 },	// P
	{ 0,0,1,0,3 },	// Q
	{ 1,0,1,3,3 },	// R
	{ 1,1,1,3,3 },	// S
	{ 0,3,3,3,3 },	// T
	{ 1,1,0,3,3 },	// U
	{ 1,1,1,0,3 },	// V
	{ 1,0,0,3,3 },	// W
	{ 0,1,1,0,3 },	// X
	{ 0,1,0,0,3 },	// Y
	{ 0,0,1,1,3 },	// Z
	{ 1,0,0,0,0 },	// 1
	{ 1,1,0,0,0 },	// 2
	{ 1,1,1,0,0 },	// 3
	{ 1,1,1,1,0 },	// 4
	{ 1,1,1,1,1 },	// 5
	{ 0,1,1,1,1 },	// 6
	{ 0,0,1,1,1 },	// 7
	{ 0,0,0,1,1 },	// 8
	{ 0,0,0,0,1 },	// 9
	{ 0,0,0,0,0 }	// 0
};

const char character2[][3] = {
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"0"
};

const char character[alphabet_len] = {
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N'
};

char str_send[10];
static uint16_t key_timer;
uint8_t count = 0, flag = 0;
uint8_t morse_buffer[5] = { 3, 3, 3, 3, 3 };

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BASE,
    _FN
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    DIT = SAFE_RANGE,
    DAH
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [_BASE] = LAYOUT(
        DIT,    DAH
    )
};

void write_buffer(uint8_t pos, uint8_t data) {
	morse_buffer[pos] = data;
}

void printbuffer(void) {
	for (uint8_t i = 0; i < 5; i++) {
		switch (morse_buffer[i]) {
			case 0:
				print("0 ");
				break;
			case 1:
				print("1 ");
				break;
			case 3:
				print("3 ");
				break;
			default:
				print("??");
		}
	}
}

void printcharacter(void) {
	for (uint8_t i = 0; i < alphabet_len; i++) {
		if (morse_buffer[0] == alphabet[i][0] &&
			morse_buffer[1] == alphabet[i][1] &&
			morse_buffer[2] == alphabet[i][2] &&
			morse_buffer[3] == alphabet[i][3] &&
			morse_buffer[4] == alphabet[i][4])
		{
			uprintf("   %c", character[i]);
			send_string(character2[i]);
		}
	}
	print("\n");
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DIT:
            if (record->event.pressed) {
				// when keycode is pressed
				//SEND_STRING(".");
				write_buffer(count,dot);
				count++;
            } else {
                // when keycode is released
            }
            break;
        case DAH:
            if (record->event.pressed) {
                // when keycode is pressed
                //SEND_STRING("-");
				write_buffer(count,dash);
				count++;
            } else {
                // when keycode is released
            }
            break;
    }
	key_timer = timer_read();
	flag = 1;
	if (count == 5) {
		count = 0;
	}
    return true;
}

void matrix_init_user(void) {
	debug_enable=true;
	debug_matrix=true;
}

void matrix_scan_user(void) {
	if ((flag) && ((timer_elapsed(key_timer)) > timeout)) {
	  // do something if more than 500ms have passed
	  	flag = 0;
		count = 0;
		printbuffer();
		printcharacter();
		for (uint8_t i = 0; i < 5; i++) {	// reset buffer
			morse_buffer[i] = 3;
		}
	} else {
	  // do something if less than 500ms have passed
	}
}

/*
bool led_update_user(led_t led_state) {
    return true;
}
*/
