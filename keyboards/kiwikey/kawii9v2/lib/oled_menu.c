// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include "oled_menu.h"
#include "oled_custom_api.h"
#include "oled_ui.h"

bool menu_is_enabled = false;
static uint8_t sub_menu = 0; // 0 means not in submenu
static uint8_t menu_cursor = MENU_START_POS;

void menu_init(void) {
    oled_clear();
    draw_line_h(0,  2, 126, true);
	draw_line_h(0, 52, 126, true);
	oled_set_cursor(6, 0);
	oled_write_P(PSTR(" SETTING "), false);
	oled_set_cursor(0, MENU_START_POS);
    oled_write_ln_P(PSTR(MENU_TITLE_01), false);
    oled_write_ln_P(PSTR(MENU_TITLE_02), false);
    oled_write_ln_P(PSTR(MENU_TITLE_03), false);
    oled_write_ln_P(PSTR(MENU_TITLE_04), false);
	oled_write_ln_P(PSTR(MENU_TITLE_05), false);
	menu_cursor = MENU_START_POS;
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

bool process_record_menu(uint16_t keycode, keyrecord_t *record) {
	if (!(record->event.pressed)) // no need to check when the key is released
		return true;
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
	if (sub_menu) {
		menu_action();
	}
    if (menu_is_enabled) {
        if (menu_cursor > MENU_NUMOFLINES) menu_cursor = MENU_START_POS;
        if (menu_cursor == 0) menu_cursor = MENU_NUMOFLINES;
        menu_set_cursor(menu_cursor);
    }
	return true;
}

void menu_action(void) {
    switch (sub_menu) {
        case 1:
			//
            break;
        case 2:
			//
            break;
        case 3:
			//
			break;
        case 4:
			action_factoryreset();
			break;
        case 5:
			reset_keyboard();
			break;
        default:
            break;
    }
	sub_menu = 0;
}

void action_factoryreset(void) {
	eeconfig_disable();
	soft_reset_keyboard();
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
