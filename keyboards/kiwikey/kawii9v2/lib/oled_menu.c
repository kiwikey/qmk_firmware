// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#if defined(OLED_ENABLE)

#include "oled_menu.h"
#include "oled_custom_api.h"
#include "oled_ui.h"

extern uint8_t	eepdata_active_layer,
				eepdata_oled_anim,
				eepdata_oled_timeout;
extern bool		eepdata_layer_indicator;

uint8_t current_menu = NOT_IN_MENU;
static uint8_t menu_execute = 0; // 0 means no "menu line" is activated/chosen
static uint8_t menu_cursor = MAINMENU_1STLINE_POS;

void menu_init(void) {
    oled_clear();
	// Draw 2 lines to separate the main Menu and the "Menu quick view"
    draw_line_h(0,  2, 126, true);
	draw_line_h(0, 52, 126, true);
	// Menu Title "SETTING"
	oled_set_cursor(6, 0);
	oled_write_P(PSTR(" SETTING "), false);
	menu_printlist();               // Print the menu list
	menu_set_cursor(menu_cursor);   // Print the cursor
	menu_quick_view(menu_cursor);   // Print the default quick view
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

bool process_record_menu(uint16_t keycode, keyrecord_t *record) {
	if (!(record->event.pressed)) // no need to check when the key is released
		return true;
	// Scrolling between Menu/Submenu items can be performed with keypad's keys, or rotary encoder
	// THE USE OF 2 ENCODERS IS NOT TESTED!
	uint8_t tmpMenu_Key = record->event.key.row * 10 + record->event.key.col;
	if(record->event.type == ENCODER_CCW_EVENT) { // Encoder rotation CCW = key Up or Left
		tmpMenu_Key = (current_menu == MAIN_MENU) ? MENU_KEY_UP : MENU_KEY_LEFT;
	}
	if(record->event.type == ENCODER_CW_EVENT) {  // Encoder rotation CW = key Down or Right
		tmpMenu_Key = (current_menu == MAIN_MENU) ? MENU_KEY_DOWN : MENU_KEY_RIGHT;
	}
	
	if (current_menu == MAIN_MENU) {
		switch (tmpMenu_Key) {
			case MENU_KEY_UP:
				menu_cursor--;
				if (menu_cursor == MAINMENU_LINESPERPAGE)
					menu_printlist();
				break;
			case MENU_KEY_DOWN:
				menu_cursor++;
				if (menu_cursor == MAINMENU_LINESPERPAGE+1)
					menu_printlist();
				break;
			// case MENU_KEY_LEFT:
				// break;
			// case MENU_KEY_RIGHT:
				// break;
			case MENU_KEY_SELECT:
				menu_execute = menu_cursor;
				break;
			case MENU_KEY_EXIT:
				current_menu = NOT_IN_MENU;
				menu_cursor = MAINMENU_1STLINE_POS;
				eeprom_update_custom(); // update all custom EEPROM values (if necessary)
#if defined(WPM_ENABLE)
				rgb_matrix_reload_from_eeprom();
#endif // defined(WPM_ENABLE)
				render_ui_frame();
				return true; // leave here
				break;            
			default:
				break;
		}
        if (menu_cursor > MAINMENU_MAXITEMS) {
			menu_cursor = 1;     // scroll back to #1
			menu_printlist();    // refresh the list
		}
        if (menu_cursor == 0) {
			menu_cursor = MAINMENU_MAXITEMS; // scroll to last item
			menu_printlist();            // refresh the list
		}
		menu_set_cursor(menu_cursor);
		if (menu_execute) {
			menu_action();
		}
	}
	else if (current_menu == SUB_MENU) {
		switch (tmpMenu_Key) {
			// case MENU_KEY_UP:
				// break;
			// case MENU_KEY_DOWN:
				// break;
			case MENU_KEY_LEFT:
				if (menu_execute == 1) { // Active layer
					if (eepdata_active_layer == ACTIVE_LAYER_MIN) eepdata_active_layer = ACTIVE_LAYER_MAX;
					else eepdata_active_layer--;
				}
				if (menu_execute == 2) { // Animation
					if (eepdata_oled_anim == 0) eepdata_oled_anim = OLED_ANIM_QTY;
					else eepdata_oled_anim--;
				}
				if (menu_execute == 3) { // OLED Timeout
					eepdata_oled_timeout -= OLED_TIMEOUT_STEP;
					if (eepdata_oled_timeout <= 0)
						eepdata_oled_timeout = OLED_TIMEOUT_NEVER;
				}
				break;
			case MENU_KEY_RIGHT:
				if (menu_execute == 1) { // Active layer
					if (eepdata_active_layer == ACTIVE_LAYER_MAX) eepdata_active_layer = ACTIVE_LAYER_MIN;
					else eepdata_active_layer++;
				}
				if (menu_execute == 2) { // Animation
					if (eepdata_oled_anim == OLED_ANIM_QTY) eepdata_oled_anim = 0;
					else eepdata_oled_anim++;
				}
				if (menu_execute == 3) { // OLED Timeout
					eepdata_oled_timeout += OLED_TIMEOUT_STEP;
					if (eepdata_oled_timeout > OLED_TIMEOUT_NEVER)
						eepdata_oled_timeout = OLED_TIMEOUT_MIN;
				}
				break;
			case MENU_KEY_SELECT:
			case MENU_KEY_EXIT:
				menu_execute = 0;
				current_menu = MAIN_MENU;
				if (menu_cursor == 1) {
					layer_move(eepdata_active_layer); // when exit "Active layer", activate that chosen layer
				}
				if (menu_cursor == 7) {
					menu_init(); // when exit "About Kawii9", need to re-render Main Menu
				}
				break;
			default:
				break;
		}
	}
	menu_quick_view(menu_cursor);
	return true;
}

void menu_quick_view(uint8_t menu_line) {
	// Clear the bottom line
	oled_set_cursor(0,7);
	oled_advance_page(true);
	// Render the quick-view
	oled_set_cursor(0,7);
	switch (menu_line) {
		case 1:
			oled_set_cursor(5,7);
			oled_write_char(0x28, false);
			oled_write_char(eepdata_active_layer + 0x30, false);
			oled_write_char(0x29, false);
			oled_write(layer_name[eepdata_active_layer], false);
			break;
		case 2:
			oled_write_align(anim_list[eepdata_oled_anim], ALIGN_CENTER, false);
			break;
		case 3:
			if (eepdata_oled_timeout == OLED_TIMEOUT_NEVER) {
				oled_write_align_P(PSTR("Always ON"), ALIGN_CENTER, false);
				break;
			}
			oled_set_cursor(5,7);
			oled_write(get_u8_str(eepdata_oled_timeout, ' '), false);
			oled_write_P(PSTR(" seconds"), false);
			break;
		case 6:
			oled_write_align_P(PSTR(FW_VERSION), ALIGN_CENTER, false);
			break;
		case 7:
			// oled_write_P(PSTR("..."), false);
			break;
		case 8:
			oled_write_align_P(PSTR("reset all settings"), ALIGN_CENTER, false);
			break;
		case 9:
			oled_write_align_P(PSTR("FW & memory update"), ALIGN_CENTER, false);
			break;
        default:
            break;
	}
	// Render 2 arrows (left & right) for "changable" settings
	if (menu_list_ischangable[menu_line-1] && (current_menu == SUB_MENU)) {
		oled_set_cursor(0,7);
		oled_write_char(0x11, false);
		oled_set_cursor(20,7);
		oled_write_char(0x10, false);
	}
}

void menu_action(void) {
    switch (menu_execute) {
        case 1:
			action_activelayer();
            break;
        case 2:
			action_animation();
            break;
        case 3:
			action_oledtimeout();
            break;
        case 4:
			//
			menu_execute = 0;
            break;
        case 5:
			//
			menu_execute = 0;
            break;
        case 6:
			// NOP
			menu_execute = 0;
            break;
        case 7:
			action_aboutkawii9();
			break;
        case 8:
			action_factoryreset();
			break;
        case 9:
			action_resettodfu();
			break;
        default:
            break;
    }
}

void action_aboutkawii9(void) {
	current_menu = SUB_MENU;
	oled_clear();
	oled_write_ln_P(PSTR("Kawii9 rev.2"), false);
	oled_write_ln_P(PSTR("made by KiwiKey"), false);
	oled_advance_page(true);
	oled_write_ln_P(PSTR("More infos:"), false);
	oled_write_ln_P(PSTR("www.kiwikey.vn/kawii9"), false);
}

void action_factoryreset(void) {
	eeprom_update_byte((uint8_t*)EEPROM_ACTIVE_LAYER, 0); // Default: Layer 0
	eeprom_update_byte((uint8_t*)EEPROM_OLED_ANIM,    1); // Default: QMK Logo
	eeprom_update_byte((uint8_t*)EEPROM_OLED_TIMEOUT, OLED_TIMEOUT_MIN); // Default: 30s
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

void action_activelayer(void) {
	current_menu = SUB_MENU;
}

void action_animation(void) {
	current_menu = SUB_MENU;
}

void action_oledtimeout(void) {
	current_menu = SUB_MENU;
}

void eeprom_update_custom(void) {
	eeprom_update_byte((uint8_t*)EEPROM_ACTIVE_LAYER, eepdata_active_layer);
	eeprom_update_byte((uint8_t*)EEPROM_OLED_ANIM,    eepdata_oled_anim);
	eeprom_update_byte((uint8_t*)EEPROM_OLED_TIMEOUT, eepdata_oled_timeout);
}

#if defined(RGB_MATRIX_ENABLE)
bool rgb_matrix_indicators_kb(void) { // showing Menu control keys in RGB Matrix
    if (!rgb_matrix_indicators_user()) {
        return false;
    }
	if (current_menu == MAIN_MENU) {
		rgb_matrix_set_color_all(RGB_BLACK);
		rgb_matrix_set_color(5,  RGB_WHITE);	// UP
		rgb_matrix_set_color(11, RGB_WHITE);	// DOWN
		rgb_matrix_set_color(8,  RGB_GREEN);	// MIDDLE (OK)
		rgb_matrix_set_color(10, RGB_RED);		// EXIT
	}
	if (current_menu == SUB_MENU) {
		rgb_matrix_set_color_all(RGB_BLACK);
		if (menu_execute != 7) // About Kawii9
		{
			rgb_matrix_set_color(7, RGB_WHITE);	// LEFT
			rgb_matrix_set_color(9, RGB_WHITE);	// RIGHT
		}
		rgb_matrix_set_color(10, RGB_RED);		// EXIT
	}
    return true;
}
#endif // defined(RGB_MATRIX_ENABLE)

#endif // defined(OLED_ENABLE)
