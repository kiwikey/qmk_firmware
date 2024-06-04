// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "kawii9v2.h"

#if defined(OLED_ENABLE)
#include "oled_key_matrix.h"
#include "oled_wpm_graph.h"
#include "oled_ui.h"
#include "oled_menu.h"
#endif // defined(OLED_ENABLE)

uint32_t key_timer = 0;
uint8_t eepdata_active_layer,
		eepdata_oled_anim,
		eepdata_oled_timeout;

void keyboard_post_init_kb(void) {

#if defined(CONSOLE_ENABLE)
	debug_enable=true;
	debug_matrix=false;
	debug_keyboard=false;
	debug_mouse=false;
#endif // defined(CONSOLE_ENABLE)

	eepdata_active_layer = eeprom_read_byte((uint8_t*)EEPROM_ACTIVE_LAYER);
	eepdata_oled_anim    = eeprom_read_byte((uint8_t*)EEPROM_OLED_ANIM);
	eepdata_oled_timeout = eeprom_read_byte((uint8_t*)EEPROM_OLED_TIMEOUT);
	// if (eepdata_active_layer) || (eepdata_oled_anim) || (eepdata_oled_timeout) { //check data validation
	// }
	layer_move(eepdata_active_layer);

#if defined(OLED_ENABLE)
    render_ui_frame();
#endif // defined(OLED_ENABLE)
    keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	if (record->event.pressed)
		key_timer = timer_read32();
#if defined(OLED_ENABLE)
    if (current_menu != NOT_IN_MENU) { // in MENU all keys are for controlling, no keycode is sent
		process_record_menu(keycode, record);
		return false;
    }

# ifdef OLED_KEY_MATRIX
	render_matrix();
# endif // OLED_KEY_MATRIX

    switch (keycode) {
        case QK_LIGHTING ... QK_LIGHTING_MAX:
            sub_ui_mode = 1;
            sub_ui_clear();
            break;
        /*** TODO
        case KC_AUDIO_MUTE ... KC_MEDIA_EJECT:
            sub_ui_mode = 2;
            sub_ui_clear();
            break;
        ***/
        case CUSTOM_KC_MENU:
            if (current_menu == NOT_IN_MENU) {
				current_menu = MAIN_MENU;
				layer_move(eepdata_active_layer); // to avoid weird behavior of current_layer when turn on MENU 
				menu_init();
			}
			return false; // no need to process this keycode
            break;
        default:
            break;
    }
#endif // OLED_ENABLE

    return process_record_user(keycode, record);
}

// void housekeeping_task_kb(void) {
	// oled_set_cursor(0,0);
	// oled_write_char(temp1+0x30,false);
	// oled_write_char(temp2+0x30,false);
// }

// Try to save some bytes...
#ifndef MAGIC_ENABLE
uint16_t keycode_config(uint16_t keycode) {  // 354 bytes
    return keycode;
}
uint8_t mod_config(uint8_t mod) {            // 206 bytes
    return mod;
}
#endif
