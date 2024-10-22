#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp_menu.h"

#include "encoder.h"
#include "eeprom_custom.h"
// #include "qp_custom_api.h"
#include "qp_graphics.h"
#include "qp_includes.h"
#include "qp_widget_layer.h"

extern painter_device_t my_display;
// extern uint8_t display_rotate_flag;
// extern bool    lcdoff_flag;
// extern bool    rgboff_flag;

uint8_t current_menu = NOT_IN_MENU;
uint8_t menu_cursor = MAINMENU_1STLINE_POS;

void menu_init(void) {
	current_menu = MAIN_MENU;
    qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_COLOR_BACKGROUND, true); // Clear screen
	qp_drawtext_recolor(my_display, 60, 10, robotobold25, "SETTINGS", HSV_CYAN, HSV_BLACK); // Menu title
	menu_printlist();               // Print the menu list
	menu_set_cursor(menu_cursor);   // Print the cursor
	menu_quick_view();              // Print the default quick view
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
    ui_refresh();
}

void menu_printlist(void) { // Print the menu list, total MAINMENU_LINESPERPAGE lines
	// Clear the old list
	qp_rect(my_display,
	        MENU_POSX, MENU_POSY,
	        MENU_WIDTH, MENU_POSY + MAINMENU_LINESPERPAGE*roboto20->line_height,
			MENU_COLOR_BACKGROUND, true);
	// Print the list based on menu_cursor
	if (menu_cursor <= MAINMENU_LINESPERPAGE) {
		for (uint8_t i = 0; i < MAINMENU_LINESPERPAGE; i++) {
			qp_drawtext(my_display, MENU_POSX, MENU_POSY+i*roboto20->line_height, roboto20, menu_list[i]);
		}
	}
	else if (menu_cursor > MAINMENU_LINESPERPAGE) {
		for (uint8_t i = MAINMENU_LINESPERPAGE; i < MAINMENU_MAXITEMS; i++) {
			qp_drawtext(my_display, MENU_POSX, MENU_POSY+(i%MAINMENU_LINESPERPAGE)*roboto20->line_height, roboto20, menu_list[i]);
		}
	}
}

void menu_set_cursor(uint8_t cursor_pos) {
	while (cursor_pos > MAINMENU_LINESPERPAGE) {
		cursor_pos -= MAINMENU_LINESPERPAGE;
	}
    for (uint8_t i = MAINMENU_1STLINE_POS; i <= MAINMENU_LINESPERPAGE; i++) {
        // qp_drawtext(my_display, MENU_POSX, MENU_POSY+(i-1)*roboto20->line_height, roboto20, i == cursor_pos ? ">" : " ");
		if (i == cursor_pos)
			qp_drawimage_recolor(my_display, MENU_POSX, MENU_POSY+2+(i-1)*roboto20->line_height, ico16_arrow_right, HSV_BLACK, HSV_YELLOW);
		else qp_drawimage_recolor(my_display, MENU_POSX, MENU_POSY+2+(i-1)*roboto20->line_height, ico16_arrow_right, HSV_BLACK, HSV_BLACK);
    }
}
void menu_quick_view(void) {
	// Clear the 2 bottom lines
	qp_rect(my_display, 0, ST7789_HEIGHT-roboto20->line_height*2, ST7789_WIDTH, ST7789_HEIGHT, MENU_COLOR_BACKGROUND, true);
	// Render the quick-view
	char buf1[50] = {0};
	switch (menu_cursor) {
		case MENU_ACTIVATELAYER:
			qp_drawtext(my_display, 10, ST7789_HEIGHT-roboto20->line_height*2, roboto20, "Turn on Layer:");
			sprintf(buf1, "[%u] %s", eepdata.active_layer, layer_name[eepdata.active_layer]);
			qp_drawtext(my_display, 40, ST7789_HEIGHT-roboto20->line_height, roboto20, buf1);
			break;
		case MENU_ANIMATION:
			// sprintf(buf1, "Animation #%u", eepdata.display_anim);
			// qp_drawtext(my_display, 40, ST7789_HEIGHT-roboto20->line_height, roboto20, buf1);
			qp_drawtext(my_display, 50, ST7789_HEIGHT-roboto20->line_height, roboto20, "unavailable");
			break;
		case MENU_DISPLAYTIMEOUT:
			qp_drawtext(my_display, 10, ST7789_HEIGHT-roboto20->line_height*2, roboto20, "Turn off LCD after");
			if (eepdata.display_timeout == DISPLAY_TIMEOUT_NEVER) {
				qp_drawtext(my_display, 50, ST7789_HEIGHT-roboto20->line_height, roboto20, "Always ON");
				break;
			}
			sprintf(buf1, "%u seconds", eepdata.display_timeout);
			qp_drawtext(my_display, 50, ST7789_HEIGHT-roboto20->line_height, roboto20, buf1);
			break;
		case MENU_DISPLAYBRIGHTNESS:
			sprintf(buf1, "%3u%%  ", eepdata.display_brightness*10);
			qp_drawtext(my_display, 90, ST7789_HEIGHT-roboto20->line_height, roboto20, buf1);
			break;
		case MENU_DISPLAYROTATION:
			sprintf(buf1, "Rotation: %u", eepdata.display_rotation*90);
			qp_drawtext(my_display, 40, ST7789_HEIGHT-roboto20->line_height, roboto20, buf1);
			break;
		case MENU_KNOBFUNCTION:
			// qp_drawtext(my_display, 10, ST7789_HEIGHT-roboto20->line_height*2, roboto20, "Rotate knob to");
			sprintf(buf1, "%s", encoder_func_name[eepdata.knob_func]);
			qp_drawtext(my_display, 20, ST7789_HEIGHT-roboto20->line_height, roboto20, buf1);
			break;
		// case MENU_LIGHTINGLAYERS:
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
		case MENU_FWVERSION:
			qp_drawtext(my_display, 40, ST7789_HEIGHT-roboto20->line_height, roboto20, FW_VERSION);
			break;
		case MENU_ABOUT:
			// oled_write_P(PSTR("..."), false);
			break;
		case MENU_FACTORYRESET:
			qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height, roboto20, " clear all settings");
			break;
		case MENU_BOOTTODFU:
			qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height*2, roboto20, "    for updating   ");
			qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height*1, roboto20, "      firmware     ");
			break;
		case MENU_DEBUG:
			// qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height*2, roboto20, "    for updating   ");
			qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height*1, roboto20, "      for nerds    ");
			break;
        default:
            break;
	}
	// Render 2 arrows (left & right) for "changable" settings
    if ((current_menu == SUB_MENU) && menu_list_ischangeable[menu_cursor]) {
		// qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height, roboto20, "<");
		// qp_drawtext(my_display, ST7789_WIDTH-roboto20->line_height, ST7789_HEIGHT-roboto20->line_height, roboto20, ">");
		qp_drawimage_recolor(my_display,
							0, (239 - ico16_arrow_left->height),
							ico16_arrow_left,  HSV_BLACK, HSV_WHITE);
		qp_drawimage_recolor(my_display,
							(239 - ico16_arrow_right->width), (239 - ico16_arrow_right->height),
							ico16_arrow_right, HSV_BLACK, HSV_WHITE);
	}
}

void menu_action(void) {
    if (menu_list_ischangeable[menu_cursor]) {
        current_menu = SUB_MENU;
		// Render 2 arrows (left & right) for "changable" settings
		// qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height, roboto20, "<");
		// qp_drawtext(my_display, ST7789_WIDTH-roboto20->line_height, ST7789_HEIGHT-roboto20->line_height, roboto20, ">");
		qp_drawimage_recolor(my_display,
							0, (239 - ico16_arrow_left->height),
							ico16_arrow_left,  HSV_BLACK, HSV_WHITE);
		qp_drawimage_recolor(my_display,
							(239 - ico16_arrow_right->width), (239 - ico16_arrow_right->height),
							ico16_arrow_right, HSV_BLACK, HSV_WHITE);
    }
    switch (menu_cursor) {
        case MENU_ACTIVATELAYER:
			// action_activelayer();
            break;
        case MENU_ANIMATION:
			// action_animation();
            break;
        case MENU_DISPLAYTIMEOUT:
			// action_displaytimeout();
            break;
        case MENU_DISPLAYBRIGHTNESS:
			// action_displaybrightness();
            break;
        case MENU_DISPLAYROTATION:
			// action_displayrotation();
            break;
        // case MENU_LIGHTINGLAYERS:
			//
            // break;
        case MENU_KNOBFUNCTION:
			//
            break;
        case MENU_FWVERSION:
			// NOP
            break;
        case MENU_ABOUT:
			action_aboutkiwi5x5();
			break;
        case MENU_FACTORYRESET:
			action_factoryreset();
			break;
        case MENU_BOOTTODFU:
			action_resettodfu();
			break;
        case MENU_DEBUG:
			action_debug();
			break;
        default:
            break;
    }
}

// void action_activelayer(void) {
	// current_menu = SUB_MENU;
// }

// void action_animation(void) {
	// current_menu = SUB_MENU;
// }

// void action_displaytimeout(void) {
	// current_menu = SUB_MENU;
// }

// void action_displaybrightness(void) {
	// current_menu = SUB_MENU;
// }

// void action_displayrotation(void) {
	// current_menu = SUB_MENU;
// }

void action_aboutkiwi5x5(void) {
	qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_COLOR_BACKGROUND, true); // Clear screen
	qp_drawtext(my_display, 0, roboto20->line_height*1, roboto20, "      Kiwi5x5      ");
	qp_drawtext(my_display, 0, roboto20->line_height*2, roboto20, "     by KiwiKey    ");
}

void action_factoryreset(void) {
	clear_keyboard();   // release all pressed keys if available
	eeprom_update_block(&eepdata_default, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
	eeconfig_disable();
	soft_reset_keyboard();
}

void action_resettodfu(void) {
	// rgb_matrix_set_color_all(RGB_BLACK);
	reset_keyboard();
}

// Maximum matrix scan frequency: 4600-4700
void action_debug(void) {
    qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_COLOR_BACKGROUND, true); // Clear screen
	qp_drawtext(my_display, 0, 0, robotobold25, "DATAS FOR NERD");
}

void eeprom_update_custom(void) {
	eeprom_update_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
