#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp_menu.h"
#include "qp_graphics.h"
#include "qp_includes.h"
#include "qp_widget_layer.h"

extern painter_device_t my_display;
extern uint8_t refresh_flag;
extern bool    lcdoff_flag;
extern bool    rgboff_flag;

uint8_t current_menu = NOT_IN_MENU;
uint8_t menu_execute = 0; // 0 means no "menu line" is being activated/chosen
static uint8_t menu_cursor = 1;

void menu_init(void) {
    qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_COLOR_BACKGROUND, true); // Clear screen
	qp_drawtext(my_display, 60, 10, robotobold25, "SETTINGS"); // Menu title
	menu_printlist();               // Print the menu list
	menu_set_cursor(menu_cursor);   // Print the cursor
	menu_quick_view(menu_cursor);   // Print the default quick view
}

void menu_printlist(void) { // Print the menu list, total MENU_LINESPERPAGE lines
	// Clear the old list
	qp_rect(my_display,
	        MENU_POSX, MENU_POSY,
	        MENU_WIDTH, MENU_POSY + MENU_LINESPERPAGE*roboto20->line_height,
			MENU_COLOR_BACKGROUND, true);
	
	// Print the list based on menu_cursor
	if (menu_cursor <= MENU_LINESPERPAGE) {
		for (uint8_t i = 0; i < MENU_LINESPERPAGE; i++) {
			qp_drawtext(my_display, MENU_POSX, MENU_POSY+i*roboto20->line_height, roboto20, menu_list[i]);
		}
	}
	else if (menu_cursor > MENU_LINESPERPAGE) {
		for (uint8_t i = MENU_LINESPERPAGE; i < MAINMENU_MAXITEMS; i++) {
			qp_drawtext(my_display, MENU_POSX, MENU_POSY+(i%MENU_LINESPERPAGE)*roboto20->line_height, roboto20, menu_list[i]);
		}
	}
}

void menu_set_cursor(uint8_t cursor_pos) {
	while (cursor_pos > MENU_LINESPERPAGE) {
		cursor_pos -= MENU_LINESPERPAGE;
	}
    for (uint8_t i = 1; i <= MENU_LINESPERPAGE; i++) {
        qp_drawtext(my_display, MENU_POSX, MENU_POSY+(i-1)*roboto20->line_height, roboto20, i == cursor_pos ? ">" : " ");
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
				if (menu_cursor == MENU_LINESPERPAGE)
					menu_printlist();
				break;
			case MENU_KEY_DOWN:
				menu_cursor++;
				if (menu_cursor == MENU_LINESPERPAGE+1)
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
				menu_cursor = 1;
				eeprom_update_custom(); // update all custom EEPROM values (if necessary)
				if (rgboff_flag) {
					rgb_matrix_disable_noeeprom();
					rgboff_flag = false;
				}
				refresh_flag = 1;
				return true; // leave here
				break;            
			default:
				return true; // leave here
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
			case MENU_KEY_UP:
				break;
			case MENU_KEY_DOWN:
				break;
			case MENU_KEY_LEFT:
				if (menu_execute == 1) { // Active layer
					if (eepdata.active_layer == ACTIVE_LAYER_MIN) eepdata.active_layer = ACTIVE_LAYER_MAX;
					else eepdata.active_layer--;
				}
				if (menu_execute == 2) { // Animation
					if (eepdata.oled_anim == 0) eepdata.oled_anim = QP_ANIM_QTY;
					else eepdata.oled_anim--;
				}
				if (menu_execute == 3) { // LCD Timeout
					eepdata.oled_timeout -= QP_TIMEOUT_STEP;
					if (eepdata.oled_timeout <= 0)
						eepdata.oled_timeout = QP_TIMEOUT_NEVER;
				}
				if (menu_execute == 4) { // LCD Brightness
					if (eepdata.oled_brightness == 1)
						eepdata.oled_brightness = BACKLIGHT_LEVELS;
					else eepdata.oled_brightness--;
					backlight_level(eepdata.oled_brightness);
				}
				if (menu_execute == 5) { // Lighting Layers
					// if (eepdata.lighting_layers == 0) {
						// eepdata.lighting_layers = 1;
						// eepdata.lighting_flags = 2;
					// } else eepdata.lighting_flags ? eepdata.lighting_flags-- : (eepdata.lighting_layers = 0);  // Drawing a flowchart may help
				}
				if (menu_execute == 6) { // Layers' Color
					// eepdata.layer_hue[eepdata.active_layer] -= OLED_ADJUSTCOLOR_STEP;
					// if (eepdata.layer_hue[eepdata.active_layer] == 242) eepdata.layer_hue[eepdata.active_layer] = 252; // Manually tune for a special case (TODO: clean up this)
					// eepdata.layer_hue[eepdata.active_layer] -= eepdata.layer_hue[eepdata.active_layer] % OLED_ADJUSTCOLOR_STEP; // Tune the HUE to "correct" color
				}
				break;
			case MENU_KEY_RIGHT:
				if (menu_execute == 1) { // Active layer
					if (eepdata.active_layer == ACTIVE_LAYER_MAX) eepdata.active_layer = ACTIVE_LAYER_MIN;
					else eepdata.active_layer++;
				}
				if (menu_execute == 2) { // Animation
					if (eepdata.oled_anim == QP_ANIM_QTY) eepdata.oled_anim = 0;
					else eepdata.oled_anim++;
				}
				if (menu_execute == 3) { // LCD Timeout
					eepdata.oled_timeout += QP_TIMEOUT_STEP;
					if (eepdata.oled_timeout > QP_TIMEOUT_NEVER)
						eepdata.oled_timeout = QP_TIMEOUT_MIN;
				}
				if (menu_execute == 4) { // LCD Brightness
					if (eepdata.oled_brightness == BACKLIGHT_LEVELS)
						eepdata.oled_brightness = 1;
					else eepdata.oled_brightness++;
					backlight_level(eepdata.oled_brightness);
				}
				if (menu_execute == 5) { // Lighting Layers
					// if (eepdata.lighting_flags == 2) {
						// eepdata.lighting_flags = 0;
						// eepdata.lighting_layers = 0;
					// } else eepdata.lighting_layers ? eepdata.lighting_flags++ : eepdata.lighting_layers++; // Drawing a flowchart may help
				}
				if (menu_execute == 6) { // Layers' Color
					// eepdata.layer_hue[eepdata.active_layer] += OLED_ADJUSTCOLOR_STEP;
					// eepdata.layer_hue[eepdata.active_layer] -= eepdata.layer_hue[eepdata.active_layer] % OLED_ADJUSTCOLOR_STEP; // Tune the HUE to "correct" color
				}
				break;
			case MENU_KEY_SELECT: // In sub-menu, 'select' key
			case MENU_KEY_EXIT:   // works as 'exit' key
				menu_execute = 0;
				current_menu = MAIN_MENU;
				if (menu_cursor == 1) {
					layer_move(eepdata.active_layer); // when exit "Active layer", activate that chosen layer
				}
				if (menu_cursor == 8) {
					menu_init(); // when exit "About ...", need to re-render Main Menu
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
	// Clear the 2 bottom lines
	qp_rect(my_display, 0, ST7789_HEIGHT-roboto20->line_height*2, ST7789_WIDTH, ST7789_HEIGHT, MENU_COLOR_BACKGROUND, true);
	
	// Render the quick-view
	char buf1[50] = {0};
	switch (menu_line) {
		case 1: // Active layer
			qp_drawtext(my_display, 10, ST7789_HEIGHT-roboto20->line_height*2, roboto20, "Turn on Layer:");
			sprintf(buf1, "[%u] %s", eepdata.active_layer, layer_name[eepdata.active_layer]);
			qp_drawtext(my_display, 40, ST7789_HEIGHT-roboto20->line_height, roboto20, buf1);
			break;
		case 2: // Animation
			sprintf(buf1, "Animation #%u", eepdata.oled_anim);
			qp_drawtext(my_display, 40, ST7789_HEIGHT-roboto20->line_height, roboto20, buf1);
			break;
		case 3: // LCD Timeout
			qp_drawtext(my_display, 10, ST7789_HEIGHT-roboto20->line_height*2, roboto20, "Turn off LCD after");
			if (eepdata.oled_timeout == QP_TIMEOUT_NEVER) {
				qp_drawtext(my_display, 40, ST7789_HEIGHT-roboto20->line_height, roboto20, "Always ON");
				break;
			}
			sprintf(buf1, "%u seconds", eepdata.oled_timeout);
			qp_drawtext(my_display, 50, ST7789_HEIGHT-roboto20->line_height, roboto20, buf1);
			break;
		case 4: // LCD Brightness
			sprintf(buf1, "%3u%%", eepdata.oled_brightness*10);
			qp_drawtext(my_display, 90, ST7789_HEIGHT-roboto20->line_height, roboto20, buf1);
			break;
		// case 5:
			// if (!eepdata.lighting_layers) {
				// oled_write_align("OFF", ALIGN_CENTER, false);
			// }
			// else switch (eepdata.lighting_flags) {
					// case 0:
						// oled_write_align("ON (Underglow)", ALIGN_CENTER, false);
						// break;
					// case 1:
						// oled_write_align("ON (Backlight)", ALIGN_CENTER, false);
						// break;
					// case 2:
						// oled_write_align("ON (All LEDs)", ALIGN_CENTER, false);
						// break;
				// }
			// break;
		// case 6:
			// oled_set_cursor(3,7);
			// if (!eepdata.lighting_layers) {
				// oled_write("(unavailable)", false);
				// break;
			// }
			// if (current_menu == MAIN_MENU) {
				// oled_write(" of Layer (", false);
				// oled_write_char(get_highest_layer(layer_state)+0x30, false);
				// oled_write_char(0x29, false); // ')'
			// }
			// else if (current_menu == SUB_MENU) {
				// oled_write("Layer (", false);
				// oled_write_char(get_highest_layer(layer_state)+0x30, false);
				// oled_write(") color", false);
			// }
			// break;
		case 7:
			qp_drawtext(my_display, 40, ST7789_HEIGHT-roboto20->line_height, roboto20, FW_VERSION);
			break;
		case 8:
			// oled_write_P(PSTR("..."), false);
			break;
		case 9:
			qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height, roboto20, " clear all settings");
			break;
		case 10:
			qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height*2, roboto20, "    for updating   ");
			qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height*1, roboto20, "      firmware     ");
			break;
        default:
            break;
	}
	// Render 2 arrows (left & right) for "changable" settings
	if (menu_list_ischangable[menu_line-1] && (current_menu == SUB_MENU)) {
		qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height, roboto20, "<");
		qp_drawtext(my_display, ST7789_WIDTH-roboto20->line_height, ST7789_HEIGHT-roboto20->line_height, roboto20, ">");
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
			action_displaytimeout();
            break;
        case 4:
			action_displaybrightness();
            break;
        case 5:
			action_lightinglayers();
            break;
        case 6:
			action_lightingconfig();
			break;
        case 7:
			// NOP
			menu_execute = 0;
            break;
        case 8:
			action_aboutkiwi5x5();
			break;
        case 9:
			action_factoryreset();
			break;
        case 10:
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

void action_displaytimeout(void) {
	current_menu = SUB_MENU;
}

void action_displaybrightness(void) {
	current_menu = SUB_MENU;
}

void action_lightinglayers(void) {
	current_menu = SUB_MENU;
}

void action_lightingconfig(void) {
	// if (eepdata.lighting_layers) { // Process to SubMenu as long as Lighting Layers is ON
		current_menu = SUB_MENU;
	// }
}

void action_aboutkiwi5x5(void) {
	current_menu = SUB_MENU;
	qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_COLOR_BACKGROUND, true); // Clear screen
	qp_drawtext(my_display, 0, roboto20->line_height*1, roboto20, "      Kiwi5x5      ");
	qp_drawtext(my_display, 0, roboto20->line_height*2, roboto20, "     by KiwiKey    ");
}

void action_factoryreset(void) {
	EEPROM_CUSTOM_DATA eepdata_default = {
		0,                           // Layer 0
		1,                           // Animation #1
		QP_TIMEOUT_MIN,              // LCD Timeout 30s
		BACKLIGHT_DEFAULT_LEVEL,     // LCD Brightness default (10 = max)
		0,                           // Lighting Layers OFF
		0,                           // Lighting Layers applied to Underglow LEDs
		{ 126, 210,  42,  84 },      // Lighting Layers' HUEs: Cyan - Magenta - Yellow - Green
		{ 255, 255, 255, 255 }       // Lighting Layers' SATs: maximum (255)
	};
	eeprom_update_block(&eepdata_default, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
	// eeconfig_disable(); // RP2040 does not require this (?)
	soft_reset_keyboard();
}

void action_resettodfu(void) {
	// oled_clear();
	// oled_set_cursor(4, 3);
	// oled_write_P(PSTR("> DFU Mode <"), false);
    // oled_render_dirty(true);
	// rgb_matrix_set_color_all(RGB_BLACK);
	reset_keyboard();
}

void eeprom_update_custom(void) {
	eeprom_update_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
}

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) { // Lighting Layers
    if (!rgb_matrix_indicators_advanced_user(led_min, led_max)) {
        return false;
    }

	// HSV hsv = {
		// eepdata.layer_hue[get_highest_layer(layer_state)], // HUE = HUE setting of current layer
		// eepdata.layer_sat[get_highest_layer(layer_state)],
		// rgb_matrix_get_val() // VAL = current RGBMatrix's brightness
	// };
	// if (hsv.s == 0) hsv.v = 0;
    // RGB rgb = hsv_to_rgb(hsv);

	if (current_menu == MAIN_MENU) {
		rgb_matrix_set_color_all(RGB_BLACK);
		rgb_matrix_set_color(17, RGB_WHITE);  // UP
		rgb_matrix_set_color(27, RGB_WHITE);  // DOWN
		rgb_matrix_set_color(22, RGB_GREEN);  // MIDDLE (OK)
		rgb_matrix_set_color(30, RGB_RED);    // EXIT
		return false;
	}
	if (current_menu == SUB_MENU) {
		// if (menu_execute == 5) { // In SubMenu 5 (Lighting Layer's Color), show the real-time color
			// for (uint8_t i = led_min; i < led_max; i++) {
				// if (g_led_config.flags[i] & ((eepdata.lighting_flags+1) << 1)) {
					// rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
				// }
			// }
			// return fal0se;
		// }
		rgb_matrix_set_color_all(RGB_BLACK);
		rgb_matrix_set_color(21, RGB_WHITE);  // LEFT
		rgb_matrix_set_color(23, RGB_WHITE);  // RIGHT
		rgb_matrix_set_color(22, RGB_GREEN);  // MIDDLE (OK)
		rgb_matrix_set_color(30, RGB_RED);    // EXIT
		return false;
	}

	// if (!(eepdata.lighting_layers & 0x0F)) // If Lighting Layers is off, there's nothing to do here
		// return false;

    // for (uint8_t i = led_min; i < led_max; i++) {
		// if (g_led_config.flags[i] & ((eepdata.lighting_flags+1) << 1)) {
            // rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        // }
    // }
    return false;
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
