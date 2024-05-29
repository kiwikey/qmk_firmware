/* Copyright 2020 LTT
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
#include "stdio.h"
#include "eeprom.h"
#include "raw_hid.h"
//#include "keycode.h"


// Following QMK tutorial: https://github.com/qmk/qmk_firmware/blob/master/docs/custom_quantum_functions.md
typedef union {
  uint32_t raw;
  struct {
    bool     rgb_layer_change :1;
  };
} user_config_t;
user_config_t user_config;

//uint8_t ByteOfData;
#define	readlen		128
uint8_t StringOfData[readlen];
uint8_t i = 0;
const uint8_t blankdata[768];
const uint8_t asciidata[5] = {23,11,18,17,10};
enum hid_keyboard_keypad_usage keysend;
uint8_t temp = 0;

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BASE = 0,
    _FN
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    xTEST = SAFE_RANGE,
    xTEST2,
	xTEST3,
	xTEST4,
	AltiumEOS
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
	[_BASE] = LAYOUT(
        KC_7,    KC_8,    KC_9,    	KC_KP_SLASH,
        KC_4,    KC_5,    KC_6,    	KC_KP_ASTERISK,
		KC_1,    KC_2,    KC_3,    	KC_KP_MINUS,
		AltiumEOS,    KC_KP_DOT,  KC_KP_ENTER,	LT(_FN, KC_KP_PLUS)
    ),
    [_FN] = LAYOUT(
        RGB_TOG,    		RGB_MODE_FORWARD,    KC_BRIU,    KC_BRID,
        LWIN(LSFT(KC_S)),	KC_WHOM,             RGB_VAI,    RGB_VAD,
		xTEST,				xTEST2,              EEP_RST,    RESET,
        xTEST3,				xTEST4,    	     KC_MUTE,    XXXXXXX
    )
};

void clear_eeprom(void) {
	print("Clear EEPROM!\n");
	eeprom_write_block((const void*)blankdata,(uint8_t*)64, 768);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case xTEST:
            if (record->event.pressed) {
                // when keycode QMKBEST is pressed
                print("Read!\n");
				eeprom_read_block((void*)&StringOfData, (const void*)0, readlen);
            } else {
                // when keycode QMKBEST is released
            }
            break;
        case xTEST2:
            if (record->event.pressed) {
                // when keycode QMKURL is pressed
                print("Print:\n");
				for (i = 0; i < readlen; i++)
				{
					if ((i%32) == 0)
						print("\n");
					uprintf("%4d", StringOfData[i]);
				}
				print("\n");
            } else {
                // when keycode QMKURL is released
            }
            break;		
        case xTEST3:
            if (record->event.pressed) {
                // when keycode QMKBEST is pressed
				//eeprom_write_block((const void *)asciidata, (void *)64, 10);
				temp = eeprom_read_byte((const uint8_t *)64);
				register_code(temp);
				unregister_code(temp);
            } else {
                // when keycode QMKBEST is released
            }
            break;
        case xTEST4:
            if (record->event.pressed) {
                // when keycode QMKBEST is pressed
				//MACRO(T(*num1to9[2]));
				//MACRO(TYPE(num1to9[1]));
				temp = eeprom_read_byte((const uint8_t *)65);
				register_code(temp);
				unregister_code(temp);
            } else {
                // when keycode QMKBEST is released
            }
            break;		
        case AltiumEOS:
            if (record->event.pressed) {
				SEND_STRING("eos");
            } else {
                // when keycode QMKBEST is released
            }
            break;				
    }
    return true;
}

led_config_t g_led_config = { {
	  // Key Matrix to LED Index
	  { 0, 1, 2, 3 },
	  { 4, 5, 6, 7 },
	  { 8, 9, 10, 11 },
	  { 12, 13, 14, 15 }
	}, {
	  // LED Index to Physical Position
	  {  0, 0 }, {  0, 75 }, {  0, 149 }, {  0, 224 },
	  { 21, 0 }, { 21, 75 }, { 21, 149 }, { 21, 224 },
	  { 42, 0 }, { 42, 75 }, { 42, 149 }, { 42, 224 },
	  { 64, 0 }, { 64, 75 }, { 64, 149 }, { 64, 224 }
	}, {
	  // LED Index to Flag
	  1, 1, 1, 1,
	  1, 1, 1, 1,
	  1, 1, 1, 1,
	  1, 1, 1, 1
	} };

/*
void keyboard_post_init_user(void) {
  // Call the keymap level matrix init.

  // Read the user config from EEPROM
  user_config.raw = eeconfig_read_user();

  // Set default layer, if enabled
  if (user_config.rgb_layer_change) {
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom_cyan();
    rgblight_mode_noeeprom(1);
  }
}
*/

void matrix_init_user(void) {
	rgb_matrix_disable();
	rgb_matrix_enable();
	rgb_matrix_sethsv(111, 255, 255);
}

/*
void matrix_scan_user(void) {

}

bool led_update_user(led_t led_state) {
    return true;
}
*/

/*
void eeconfig_init_user(void) {  // EEPROM is getting reset!
  user_config.raw = 0;
  user_config.rgb_layer_change = true; // We want this enabled by default
  eeconfig_update_user(user_config.raw); // Write default value to EEPROM now

  // use the non noeeprom versions, to write these values to EEPROM too
  rgblight_enable(); // Enable RGB by default
  rgblight_sethsv_cyan();  // Set it to CYAN by default
  rgblight_mode(1); // set to solid by default
}
*/

void raw_hid_receive(uint8_t *data, uint8_t length) {
	// Create data array, size 64 bytes, and fill with 0x00 (to avoid weird data)
	uint8_t dataHIDhandshake[64];
	uint8_t dataHIDlayout[64];
	for (int i = 0; i < 64; i++) {
		dataHIDhandshake[i] = 0x00;
		dataHIDlayout[i] = 0x00;
	}
	
	// Define response package
	dataHIDhandshake[0] = 0xAA;
	for (int i = 1; i <= 9; i++) {
		dataHIDlayout[i] = i+5;
	}
	
	if (data[0] == 0x55) {
		raw_hid_send(dataHIDhandshake, length);		// If receive 0x55, confirm handshake
		rgblight_step();
	}
	
	if (data[0] == 0x69) {
		raw_hid_send(dataHIDlayout, length);		// If receive 0x69, send layout
		rgblight_step();
	}
	else {
		raw_hid_send(data, length);					// Or else, just echo what received
	}
	
	if (data[0] == 0x99) {
		eeprom_write_byte((uint8_t *)64, data[1]);
		eeprom_write_byte((uint8_t *)65, data[2]);
	}
}