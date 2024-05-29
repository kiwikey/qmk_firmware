// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include "oled_menu.h"
#include "oled_custom_api.h"
#include "oled_ui.h"

uint8_t current_menu = NOT_IN_MENU;
static uint8_t menu_execute = 0; // 0 means no "menu line" is activated/chosen
static uint8_t menu_cursor = MENU_1STLINE_POS;

void menu_init(void) {
    oled_clear();
	// Draw 2 lines to separate the main Menu and the "Menu quick view"
    draw_line_h(0,  2, 126, true);
	draw_line_h(0, 52, 126, true);
	// Menu Title "SETTING"
	oled_set_cursor(6, 0);
	oled_write_P(PSTR(" SETTING "), false);
	// Print the menu list, total MENU_NUMOFLINES lines
	oled_set_cursor(0, MENU_1STLINE_POS);
	for (uint8_t i = 0; i < MENU_NUMOFLINES; i++) {
		oled_write_ln(menu_list[i], false);	
	}
	// Print the default cursor (position 0)
	menu_cursor = MENU_1STLINE_POS;
    menu_set_cursor(menu_cursor);
	// Print the default quick view
	menu_quick_view(menu_cursor);
}

void menu_set_cursor(uint8_t cursor_pos) {
    for (uint8_t i = MENU_1STLINE_POS; i < MENU_NUMOFLINES+1; i++) {
        oled_set_cursor(0,i);
        if (i == cursor_pos)
            oled_write_char(0x10, false);  // space
        else oled_write_char(0x20, false); // >
    }
}

bool process_record_menu(uint16_t keycode, keyrecord_t *record) {
	if (!(record->event.pressed)) // no need to check when the key is released
		return true;
	if (current_menu == MAIN_MENU) {
		switch (record->event.key.row * 10 + record->event.key.col) {
			case MENU_KEY_UP:
				menu_cursor--;
				break;
			case MENU_KEY_DOWN:
				menu_cursor++;
				break;
			case MENU_KEY_LEFT:
				
				break;
			case MENU_KEY_RIGHT:
				
				break;
			case MENU_KEY_SELECT:
				menu_execute = menu_cursor;
				break;
			case MENU_KEY_EXIT:
				current_menu = NOT_IN_MENU;
				rgb_matrix_reload_from_eeprom();
				render_ui_frame();
				return true; // leave here
				break;            
			default:
				break;
		}
        if (menu_cursor > MENU_NUMOFLINES) menu_cursor = MENU_1STLINE_POS;
        if (menu_cursor == 0) menu_cursor = MENU_NUMOFLINES;
        menu_set_cursor(menu_cursor);
		menu_quick_view(menu_cursor);
		menu_quick_view(menu_cursor);
		if (menu_execute) {
			menu_action();
		}
	}
	else if (current_menu == SUB_MENU) {
		switch (record->event.key.row * 10 + record->event.key.col) {
			// case MENU_KEY_UP:
				
				// break;
			// case MENU_KEY_DOWN:
				
				// break;
			// case MENU_KEY_LEFT:
				
				// break;
			// case MENU_KEY_RIGHT:
				
				// break;
			// case MENU_KEY_SELECT:
				
				// break;
			case MENU_KEY_EXIT:
				current_menu = MAIN_MENU;
				menu_init();
				return true; // leave here
				break;            
			default:
				break;
		}
	}
	return true;
}

void menu_quick_view(uint8_t menu_line) {
	// Clear the bottom line
	oled_set_cursor(0,7);
	oled_advance_page(true);
	// Render the quick-view
	oled_set_cursor(2,7);
	switch (menu_line) {
		case 1:
			oled_write_P(PSTR("Nothing special"), false);
			break;
		case 2:
			oled_write(get_u16_str(OLED_TIMEOUT/1000, ' '), false);
			oled_write_P(PSTR(" seconds"), false);
			break;
		case 3:
			oled_write_P(PSTR("FW: "), false);
			oled_write_P(PSTR(FW_VERSION), false);
			break;
		case 4:
			oled_write_P(PSTR("Clear all"), false);
			break;
		case 5:
			oled_write_P(PSTR("For updating FW"), false);
			break;
        default:
            break;
	}
	if (menu_list_ischangable[menu_line-1]) {
		oled_set_cursor(0,7);
		oled_write_char(0x11, false);
		oled_set_cursor(20,7);
		oled_write_char(0x10, false);
	}
}

void menu_action(void) {
    switch (menu_execute) {
        case 1:
			action_aboutkawii9();
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
			action_resettodfu();
			break;
        default:
            break;
    }
	menu_execute = 0;
}

void action_aboutkawii9(void) {
	current_menu = SUB_MENU;
	oled_clear();
	oled_write_ln_P(PSTR("Kawii9 rev.2"), false);
	oled_write_ln_P(PSTR("made by KiwiKey"), false);
	oled_advance_page(true);
	oled_write_ln_P(PSTR("More infos:"), false);
	oled_write_ln_P(PSTR("www.kiwikey.vn/kawii9"), false);
	oled_advance_page(true);
	oled_write_P(PSTR("FW "), false);
	oled_write_ln_P(PSTR(FW_VERSION), false);
}

void action_factoryreset(void) {
	eeconfig_disable();
	soft_reset_keyboard();
}

void action_resettodfu(void) {
	oled_clear();
	oled_set_cursor(4, 3);
	oled_write_P(PSTR("> DFU Mode <"), false);
    oled_render_dirty(true);
	// rgb_matrix_set_color_all(RGB_BLACK);
	reset_keyboard();
}

bool rgb_matrix_indicators_kb(void) { // showing Menu control keys in RGB Matrix
    if (!rgb_matrix_indicators_user()) {
        return false;
    }
	if (current_menu == MAIN_MENU) {
		rgb_matrix_set_color_all(RGB_BLACK);
		rgb_matrix_set_color(5,  RGB_WHITE);	// UP
		rgb_matrix_set_color(11, RGB_WHITE);	// DOWN
		rgb_matrix_set_color(7,  RGB_WHITE);	// LEFT
		rgb_matrix_set_color(9,  RGB_WHITE);	// RIGHT
		rgb_matrix_set_color(8,  RGB_GREEN);	// MIDDLE (OK)
		rgb_matrix_set_color(10, RGB_RED);		// EXIT
	}
	if (current_menu == SUB_MENU) {
		rgb_matrix_set_color_all(RGB_BLACK);
		rgb_matrix_set_color(10, RGB_RED);		// EXIT
	}
    return true;
}
