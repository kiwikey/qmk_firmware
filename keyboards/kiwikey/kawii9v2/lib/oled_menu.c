// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include "oled_menu.h"
#include "oled_ui.h"

bool menu_is_enabled = false;
static uint8_t sub_menu = 0; // 0 means not in submenu
static uint8_t menu_cursor = MENU_START_POS;

void menu_init(void) {
	menu_cursor = MENU_START_POS;
    oled_clear();
    oled_write_P(PSTR(MENU_TOP_LINE), false);
    oled_write_ln_P(PSTR(MENU_TITLE_01), false);
    oled_write_ln_P(PSTR(MENU_TITLE_02), false);
    oled_write_ln_P(PSTR(MENU_TITLE_03), false);
    oled_write_P(PSTR(MENU_TITLE_04), false);
    // oled_write_P(PSTR(FW_VERSION), false);
    menu_set_cursor(menu_cursor);
}

void menu_set_cursor(uint8_t cursor_pos) {
    for (uint8_t i = MENU_START_POS; i < MENU_NUMOFLINES+1; i++) {
        oled_set_cursor(0,i);
        if (i == cursor_pos)
            oled_write_char(0x10, false);  // space
        else oled_write_char(0x20, false); // >
    }
}

void process_record_menu(uint16_t keycode, keyrecord_t *record) {
    switch (record->event.key.row * 10 + record->event.key.col) {
        case MENU_KEY_UP:
            menu_cursor--;
            break;
        case MENU_KEY_DOWN:
            menu_cursor++;
            break;
        case MENU_KEY_OK:
            sub_menu = menu_cursor;
            break;
        case MENU_KEY_EXIT:
            menu_is_enabled = false;
			rgb_matrix_reload_from_eeprom();
            render_ui_frame();
            break;            
        default:
            break;
    }
    if (menu_is_enabled) {
        if (menu_cursor > MENU_NUMOFLINES) menu_cursor = MENU_START_POS;
        if (menu_cursor == 0) menu_cursor = MENU_NUMOFLINES;
        menu_set_cursor(menu_cursor);
    }
}

bool rgb_matrix_indicators_kb(void) { // showing Menu control keys in RGB Matrix
    if (!rgb_matrix_indicators_user()) {
        return false;
    }
	if (menu_is_enabled) {
		rgb_matrix_set_color_all(RGB_BLACK);
		rgb_matrix_set_color(5,  RGB_CYAN);		// UP
		rgb_matrix_set_color(11, RGB_CYAN);		// DOWN
		rgb_matrix_set_color(8,  RGB_GREEN);	// MIDDLE (OK)
		rgb_matrix_set_color(10, RGB_RED);		// EXIT
	}
    return true;
}
