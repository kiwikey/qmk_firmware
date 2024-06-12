// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include "kawii9v2.h"

#if defined(OLED_ENABLE)

#include "oled_menu.h"
#include "oled_custom_api.h"
#include "oled_ui.h"

uint8_t current_menu = NOT_IN_MENU;
uint8_t menu_execute = 0; // 0 means no "menu line" is being activated/chosen
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
					if (eepdata.active_layer == ACTIVE_LAYER_MIN) eepdata.active_layer = ACTIVE_LAYER_MAX;
					else eepdata.active_layer--;
				}
				if (menu_execute == 2) { // Animation
					if (eepdata.oled_anim == 0) eepdata.oled_anim = OLED_ANIM_QTY;
					else eepdata.oled_anim--;
				}
				if (menu_execute == 3) { // OLED Timeout
					eepdata.oled_timeout -= OLED_TIMEOUT_STEP;
					if (eepdata.oled_timeout <= 0)
						eepdata.oled_timeout = OLED_TIMEOUT_NEVER;
				}
				if (menu_execute == 4) { // Lighting Layers
					if (eepdata.lighting_layers == 0) {
						eepdata.lighting_layers = 1;
						eepdata.lighting_flags = 2;
					} else eepdata.lighting_flags ? eepdata.lighting_flags-- : (eepdata.lighting_layers = 0);  // Drawing a flowchart may help
				}
				if (menu_execute == 5) { // Layers' Color
					eepdata.layer_hue[eepdata.active_layer] -= OLED_ADJUSTCOLOR_STEP;
					if (eepdata.layer_hue[eepdata.active_layer] == 242) eepdata.layer_hue[eepdata.active_layer] = 252; // Manually tune for a special case (TODO: clean up this)
					eepdata.layer_hue[eepdata.active_layer] -= eepdata.layer_hue[eepdata.active_layer] % OLED_ADJUSTCOLOR_STEP; // Tune the HUE to "correct" color
				}
				break;
			case MENU_KEY_RIGHT:
				if (menu_execute == 1) { // Active layer
					if (eepdata.active_layer == ACTIVE_LAYER_MAX) eepdata.active_layer = ACTIVE_LAYER_MIN;
					else eepdata.active_layer++;
				}
				if (menu_execute == 2) { // Animation
					if (eepdata.oled_anim == OLED_ANIM_QTY) eepdata.oled_anim = 0;
					else eepdata.oled_anim++;
				}
				if (menu_execute == 3) { // OLED Timeout
					eepdata.oled_timeout += OLED_TIMEOUT_STEP;
					if (eepdata.oled_timeout > OLED_TIMEOUT_NEVER)
						eepdata.oled_timeout = OLED_TIMEOUT_MIN;
				}
				if (menu_execute == 4) { // Lighting Layers
					if (eepdata.lighting_flags == 2) {
						eepdata.lighting_flags = 0;
						eepdata.lighting_layers = 0;
					} else eepdata.lighting_layers ? eepdata.lighting_flags++ : eepdata.lighting_layers++; // Drawing a flowchart may help
				}
				if (menu_execute == 5) { // Layers' Color
					eepdata.layer_hue[eepdata.active_layer] += OLED_ADJUSTCOLOR_STEP;
					eepdata.layer_hue[eepdata.active_layer] -= eepdata.layer_hue[eepdata.active_layer] % OLED_ADJUSTCOLOR_STEP; // Tune the HUE to "correct" color
				}
				break;
			case MENU_KEY_SELECT:
			case MENU_KEY_EXIT:
				menu_execute = 0;
				current_menu = MAIN_MENU;
				if (menu_cursor == 1) {
					layer_move(eepdata.active_layer); // when exit "Active layer", activate that chosen layer
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
			oled_write_char(0x28, false); // '('
			oled_write_char(eepdata.active_layer + 0x30, false);
			oled_write_char(0x29, false); // ')'
			oled_write(layer_name[eepdata.active_layer], false);
			break;
		case 2:
			oled_write_align(anim_list[eepdata.oled_anim], ALIGN_CENTER, false);
			break;
		case 3:
			if (eepdata.oled_timeout == OLED_TIMEOUT_NEVER) {
				oled_write_align_P(PSTR("Always ON"), ALIGN_CENTER, false);
				break;
			}
			oled_set_cursor(5,7);
			oled_write(get_u8_str(eepdata.oled_timeout, ' '), false);
			oled_write_P(PSTR(" seconds"), false);
			break;
		case 4:
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
		case 5:
			oled_set_cursor(3,7);
			if (!eepdata.lighting_layers) {
				oled_write("(unavailable)", false);
				break;
			}
			if (current_menu == MAIN_MENU) {
				oled_write(" of Layer (", false);
				oled_write_char(get_highest_layer(layer_state)+0x30, false);
				oled_write_char(0x29, false); // ')'
			}
			else if (current_menu == SUB_MENU) {
				oled_write("Layer (", false);
				oled_write_char(get_highest_layer(layer_state)+0x30, false);
				oled_write(") color", false);
			}
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
			action_lightinglayers();
            break;
        case 5:
			action_lightingconfig();
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

void action_activelayer(void) {
	current_menu = SUB_MENU;
}

void action_animation(void) {
	current_menu = SUB_MENU;
}

void action_oledtimeout(void) {
	current_menu = SUB_MENU;
}

void action_lightinglayers(void) {
	current_menu = SUB_MENU;
}

void action_lightingconfig(void) {
	if (eepdata.lighting_layers) { // Process to SubMenu as long as Lighting Layers is ON
		current_menu = SUB_MENU;
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
	EEPROM_CUSTOM_DATA eepdata_default = {
		0,                           // Layer 0
		1,                           // QMK Logo
		OLED_TIMEOUT_MIN,            // OLED Timeout 30s
		0,                           // Lighting Layers OFF
		0,                           // Lighting Layers applied to Underglow LEDs
		{ 126, 210,  42,  84, 168 }, // Lighting Layers' HUEs: Cyan - Magenta - Yellow - Green - Blue
		{ 255, 255, 255, 255, 255 }  // Lighting Layers' SATs: maximum (255)
	};
	eeprom_update_block(&eepdata_default, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
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

void eeprom_update_custom(void) {
	eeprom_update_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
}

#endif // defined(OLED_ENABLE)
