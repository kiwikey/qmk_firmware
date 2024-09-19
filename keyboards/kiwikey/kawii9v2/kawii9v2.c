// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kawii9v2.h"

#if defined(OLED_ENABLE)
    #include "oled.h"
    #include "encoder.h"
    #include "eeprom_custom.h"
    #include "oled/oled_ui.h"
    #include "oled/oled_menu.h"
    #include "oled/oled_key_matrix.h"
#endif // defined(OLED_ENABLE)

#if defined(CONSOLE_ENABLE)
    #include "print.h"
#endif // defined(CONSOLE_ENABLE)

EEPROM_CUSTOM_DATA eepdata;

void keyboard_post_init_kb(void) {
    // Reading all EEPROM custom datas, refer to 'eeprom_custom.h' for detail
    eeprom_read_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));

    // TODO: Validation check?
    layer_move(eepdata.active_layer);

    #if defined(OLED_ENABLE)
        ui_refresh();
    #endif // defined(OLED_ENABLE)

    #if defined(CONSOLE_ENABLE)
        debug_enable   = true;
        debug_matrix   = false;
        debug_keyboard = false;
        debug_mouse    = false;
    #endif // defined(CONSOLE_ENABLE)

    keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    
    #if defined(CONSOLE_ENABLE)
        // For debugging, if needed
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
