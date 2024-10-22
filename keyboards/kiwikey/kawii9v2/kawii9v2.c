// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kawii9v2.h"
#include "eeprom_custom.h"
#include "via.h"

#if defined(OLED_ENABLE)
    #include "oled.h"
    #include "encoder.h"
    #include "oled/oled_ui.h"
    #include "oled/oled_menu.h"
    #include "oled/oled_key_matrix.h"
#endif // defined(OLED_ENABLE)

#if defined(CONSOLE_ENABLE)
    #include "print.h"
#endif // defined(CONSOLE_ENABLE)

EEPROM_CUSTOM_DATA eepdata;
EEPROM_CUSTOM_DATA eepdata_default = {
	0,                              // active_layer       : 0
	1,                              // display_anim       : QMK Logo
	30,                             // display_timeout    : 30s
	0,                              // display_brightness :
	0,                              // display_rotation   : (OLED_ROTATION_0 = 0)
	0,                              // lighting_layers    : OFF
	0,                              // lighting_flags     : applied to Underglow LEDs
	{ 126, 210,  42,  84, 168 },    // layer_hue          : Cyan - Magenta - Yellow - Green - Blue
	{ 255, 255, 255, 255, 255 },    // layer_sat          : maximum (255)
	1,                              // knob_func          : Volume
	7,                              // checksum           : 7 (always)
};

void keyboard_post_init_kb(void) {
    #if defined(CONSOLE_ENABLE)
		debug_enable   = true;  // everything
		debug_matrix   = false; // col/row datas
		debug_keyboard = false; // ???
		debug_mouse    = false; // ???
    #endif // defined(CONSOLE_ENABLE)
	
    // Reading all EEPROM custom datas, refer to 'eeprom_custom.h' for detail
    eeprom_read_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));

	/*** Validation check ***/
	/* This runs everytime the EEPROM is corrupted, or right after 'factory_reset' or 'bootmagic_reset' */
	if (eepdata.checksum != 7) {
		eeprom_update_block(&eepdata_default, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
		// Reading all EEPROM custom datas, again
		eeprom_read_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
	}

    layer_move(eepdata.active_layer);

    #if defined(OLED_ENABLE)
        ui_refresh();
    #endif // defined(OLED_ENABLE)
	
    keyboard_post_init_user();
}
		
bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    
    #if defined(CONSOLE_ENABLE)
		// dprintf("Layer: %u \n",eepdata.active_layer);
		// dprintf("OLED:  %u %u %u %u\n",eepdata.display_anim, eepdata.display_timeout, eepdata.display_brightness, eepdata.display_rotation);
		// dprintf("L.L.:  %u %u\n",eepdata.lighting_layers, eepdata.lighting_flags);
		// dprintf("Knob:  %u \n",eepdata.knob_func);
		// dprintf("Check: %u \n",eepdata.checksum);
		// dprintf("EECONFIG_SIZE                 : %u \n",EECONFIG_SIZE);
		// dprintf("VIA_EEPROM_LAYOUT_OPTIONS_SIZE: %u byte\n",VIA_EEPROM_LAYOUT_OPTIONS_SIZE);
		// dprintf("VIA_EEPROM_CUSTOM_CONFIG_SIZE : %u \n",VIA_EEPROM_CUSTOM_CONFIG_SIZE);
		// dprintf("VIA            : %X - %X \n",VIA_EEPROM_MAGIC_ADDR, VIA_EEPROM_CUSTOM_CONFIG_ADDR-1);
		// dprintf("Custom config  : %X - %X \n",VIA_EEPROM_CUSTOM_CONFIG_ADDR, VIA_EEPROM_CUSTOM_CONFIG_ADDR+VIA_EEPROM_CUSTOM_CONFIG_SIZE-1 );
		// dprintf("Dynamic keymap : %u - %u \n",DYNAMIC_KEYMAP_EEPROM_ADDR, DYNAMIC_KEYMAP_MACRO_EEPROM_ADDR-1);
		// dprintf("Macros         : %u - %u \n",DYNAMIC_KEYMAP_MACRO_EEPROM_ADDR, DYNAMIC_KEYMAP_MACRO_EEPROM_ADDR+DYNAMIC_KEYMAP_MACRO_EEPROM_SIZE-1);
    #endif // defined(CONSOLE_ENABLE)
    
    #if defined(OLED_ENABLE)
        if (process_record_oled(keycode, record) == false) // process_record_oled = false if being in Menu
            return false;            
    #endif // OLED_ENABLE

    return process_record_user(keycode, record);
}

bool encoder_update_kb(uint8_t index, bool clockwise) { // Rotating
    if (!encoder_update_user(index, clockwise)) {
        return false; /* Don't process further events if user function exists and returns false */
    }
    process_encoder_rotate(index, clockwise);
    return true;
}

// AVR firmware size reducing
#ifndef MAGIC_ENABLE
uint16_t keycode_config(uint16_t keycode) {  // 354 bytes
    return keycode;
}
uint8_t mod_config(uint8_t mod) {            // 206 bytes
    return mod;
}
#endif
