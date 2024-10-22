// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include "oled_menu.h"

#include "oled.h"
#include "encoder.h"
#include "eeprom_custom.h"
#include "oled_custom_api.h"
#include "oled_ui.h"

uint8_t current_menu = NOT_IN_MENU;
uint8_t menu_cursor = MAINMENU_1STLINE_POS;

void menu_init(void) {
	clear_keyboard(); // release all pressed keys if available
    current_menu = MAIN_MENU;
    oled_clear();
    // Draw 2 lines to separate the main Menu and the "Menu quick view"
    draw_line_h(0,  2, 126, true);
    draw_line_h(0, 52, 126, true);
    // Menu Title "SETTING"
    oled_set_cursor(6, 0);
    oled_write_P(PSTR(" SETTING "), false);
    menu_printlist();             // Print the menu list
    menu_set_cursor(menu_cursor); // Print the cursor
    menu_quick_view();            // Print the default quick view
}

void menu_exit(void) {
    current_menu = NOT_IN_MENU;
    menu_cursor  = MAINMENU_1STLINE_POS;
    eeprom_update_custom(); // update all custom EEPROM values (if necessary)
    /* Special cases handling */
    if (menu_cursor == 1) { // when exit "Active layer", move to that chosen layer
        layer_move(eepdata.active_layer);
    }
    /*************************/
	clear_keyboard(); // release all pressed keys if available
    ui_refresh();
}

void menu_printlist(void) { // Print the menu list, total MAINMENU_LINESPERPAGE lines
    // Clear the old list
    oled_set_cursor(0, MAINMENU_1STLINE_POS);
    for (uint8_t i = 0; i < MAINMENU_LINESPERPAGE; i++) oled_advance_page(true);
    // Print the list based on menu_cursor
    oled_set_cursor(0, MAINMENU_1STLINE_POS);
    if (menu_cursor <= MAINMENU_LINESPERPAGE) {
        for (uint8_t i = 0; i < MAINMENU_LINESPERPAGE; i++) {
            oled_write_ln(menu_list[i], false);
        }
    }
    else if (menu_cursor > MAINMENU_LINESPERPAGE) {
        for (uint8_t i = MAINMENU_LINESPERPAGE; i < 9; i++) {
            oled_write_ln(menu_list[i], false);
        }
    }
}

void menu_set_cursor(uint8_t cursor_pos) {
    while (cursor_pos > MAINMENU_LINESPERPAGE) {
        cursor_pos -= MAINMENU_LINESPERPAGE;
    }
    for (uint8_t i = MAINMENU_1STLINE_POS; i <= MAINMENU_LINESPERPAGE; i++) {
        oled_set_cursor(0,i);
        if (i == cursor_pos)
            oled_write_char(0x10, false);  // space
        else oled_write_char(0x20, false); // >
    }
}

void menu_quick_view(void) {
    // Clear the bottom line
    oled_set_cursor(0,7);
    oled_advance_page(true);
    // Render the quick-view
    oled_set_cursor(0,7);
    switch (menu_cursor) {
        case MENU_ACTIVATELAYER:
            oled_set_cursor(5,7);
            oled_write_char(0x28, false); // '('
            oled_write_char(eepdata.active_layer + 0x30, false);
            oled_write_char(0x29, false); // ')'
            oled_write(layer_name[eepdata.active_layer], false);
            break;
        case MENU_ANIMATION:
            oled_write_align(anim_list[eepdata.display_anim], ALIGN_CENTER, false);
            break;
        case MENU_DISPLAYTIMEOUT:
            if (eepdata.display_timeout == DISPLAY_TIMEOUT_NEVER) {
                oled_write_align_P(PSTR("Always ON"), ALIGN_CENTER, false);
                break;
            }
            oled_set_cursor(5,7);
            oled_write(get_u8_str(eepdata.display_timeout, ' '), false);
            oled_write_P(PSTR(" seconds"), false);
            break;
        case MENU_LIGHTINGLAYERS:
            if (!eepdata.lighting_layers) {
                oled_write_align("OFF", ALIGN_CENTER, false);
            }
            else switch (eepdata.lighting_flags) {
                    case 0:
                        oled_write_align("ON (Underglow)", ALIGN_CENTER, false);
                        break;
                    case 1:
                        oled_write_align("ON (Backlight)", ALIGN_CENTER, false);
                        break;
                    case 2:
                        oled_write_align("ON (All LEDs)", ALIGN_CENTER, false);
                        break;
                }
            break;
        case MENU_KNOBFUNCTION:
			// oled_set_cursor(3,7);
			// oled_write(encoder_func_name[eepdata.knob_func], false);
			oled_write_align(encoder_func_name[eepdata.knob_func], ALIGN_CENTER, false);
            break;
        case MENU_FWVERSION:
            oled_write_align_P(PSTR(FW_VERSION), ALIGN_CENTER, false);
            break;
        case MENU_ABOUT:
            // oled_write_P(PSTR("..."), false);
            break;
        case MENU_FACTORYRESET:
            oled_write_align_P(PSTR("reset all settings"), ALIGN_CENTER, false);
            break;
        case MENU_BOOTTODFU:
            oled_write_align_P(PSTR("  update firmware "), ALIGN_CENTER, false);
            break;
        default:
            break;
    }
    // Render 2 arrows (left & right) for "changeable" settings
    if ((current_menu == SUB_MENU) && menu_list_ischangeable[menu_cursor]) {
        oled_set_cursor(0,7);
        oled_write_char(0x11, false);
        oled_set_cursor(20,7);
        oled_write_char(0x10, false);
    }
}

void menu_action(void) {
    if (menu_list_ischangeable[menu_cursor]) {
        current_menu = SUB_MENU;
        // Render 2 arrows (left & right) for "changeable" settings (first time)
        oled_set_cursor(0,7);
        oled_write_char(0x11, false);
        oled_set_cursor(20,7);
        oled_write_char(0x10, false);        
    }
    switch (menu_cursor) {
        case MENU_ACTIVATELAYER:
            // action_activelayer();
            break;
        case MENU_ANIMATION:
            // action_animation();
            break;
        case MENU_DISPLAYTIMEOUT:
            // action_oledtimeout();
            break;
        case MENU_LIGHTINGLAYERS:
            // action_lightinglayers();
            break;
        case MENU_KNOBFUNCTION:
            //...
            break;
        case MENU_FWVERSION:
            // NOP
            break;
        case MENU_ABOUT:
            action_aboutkawii9();
            break;
        case MENU_FACTORYRESET:
            action_factoryreset();
            break;
        case MENU_BOOTTODFU:
            action_resettodfu();
            break;
        default:
            break;
    }
}

// void action_activelayer(void) {
// }

// void action_animation(void) {
// }

// void action_oledtimeout(void) {
// }

// void action_lightinglayers(void) {
// }

// void action_knobfunction(void) {
// }

void action_aboutkawii9(void) {
    oled_clear();
    oled_write_ln_P(PSTR("Kawii9 rev.2"), false);
    oled_write_ln_P(PSTR("made by KiwiKey"), false);
    oled_advance_page(true);
    oled_write_ln_P(PSTR("More infos:"), false);
    oled_write_ln_P(PSTR("www.kiwikey.vn/kawii9"), false);
}

void action_factoryreset(void) {
    eeprom_update_block(&eepdata_default, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
    eeconfig_disable();
	clear_keyboard(); // release all pressed keys if available
    soft_reset_keyboard();
}

void action_resettodfu(void) {
    oled_clear();
    oled_set_cursor(4, 3);
    oled_write_P(PSTR("> DFU Mode <"), false);
    oled_render_dirty(true);
    rgb_matrix_set_color_all(RGB_BLACK);
    reset_keyboard();
}

void eeprom_update_custom(void) { // TODO: move to somewhere else
    eeprom_update_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
}
