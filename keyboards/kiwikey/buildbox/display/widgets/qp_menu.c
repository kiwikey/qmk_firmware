#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp_menu.h"

#include "features/eeprom_custom.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"
#include "sensor/sensors_handler.h"
#include "display/widgets/qp_widget_breakout.h"
// #include "display/widgets/qp_widget_matrix.h"
// #include "display/widgets/qp_widget_layer.h"
// #include "display/widgets/qp_widget_knob.h"

extern painter_device_t my_display;
// extern uint8_t display_rotate_flag;
// extern bool    lcdoff_flag;
// extern bool    rgboff_flag;

uint8_t menu_state  = NOT_IN_MENU;
uint8_t menu_cursor = MAINMENU_1STLINE_POS;

void menu_init(void) {
	menu_state = MAIN_MENU;
	accumulator = 0;
	qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_COLOR_BACKGROUND, true); // Clear screen
	qp_drawtext_recolor_center(my_display,
							   ST7789_WIDTH/2,
							   20,
							   robotobold25,
							   "SETTINGS",
							   HSV_CYAN, HSV_BLACK); // Menu title
	menu_printlist();               // Print the menu list
	menu_set_cursor(menu_cursor);   // Print the cursor
	// menu_quick_view();              // Print the default quick view
	qp_flush(my_display);
}

void menu_submenu_exit(void) { // Return from Sub Menu to Main Menu without a full-screen redraw
	menu_state = MAIN_MENU;
	// Erase only the arrow icons drawn when entering the sub menu
	qp_rect(my_display,
	        0, (239 - ico16_arrow_left->height),
	        ico16_arrow_left->width, 239,
	        MENU_COLOR_BACKGROUND, true);
	qp_rect(my_display,
	        (239 - ico16_arrow_right->width), (239 - ico16_arrow_right->height),
	        239, 239,
	        MENU_COLOR_BACKGROUND, true);
	// Refresh just the current row, in case its value changed in the sub menu
	menu_set_cursor(menu_cursor);
	qp_flush(my_display);
}

void menu_exit(void) {
    menu_state  = NOT_IN_MENU;
	accumulator = 0;
    menu_cursor = MAINMENU_1STLINE_POS;
    // eeprom_update_custom(); // update all custom EEPROM values (if necessary)
    /* Special cases handling */
    if (menu_cursor == 1) { // when exit "Active layer", move to that chosen layer
        // layer_move(eepdata.active_layer);
    }
    /*************************/
    ui_refresh();
}

void menu_printlist(void) { // Print the menu list, total MAINMENU_LINESPERPAGE lines
	// Clear the old list
	qp_rect(my_display,
	        MENU_POSX, MENU_POSY,
	        MENU_WIDTH, MENU_POSY + MAINMENU_LINESPERPAGE * MENU_LINE_HEIGHT,
			MENU_COLOR_BACKGROUND,
			true);
	// Print the page that contains menu_cursor (handles any item count / page size)
	uint8_t page_start = ((menu_cursor - 1) / MAINMENU_LINESPERPAGE) * MAINMENU_LINESPERPAGE;
	uint8_t page_end   = page_start + MAINMENU_LINESPERPAGE;
	if (page_end > MAINMENU_MAXITEMS) page_end = MAINMENU_MAXITEMS;

	for (uint8_t i = page_start; i < page_end; i++) {
		qp_drawtext(my_display,
					MENU_POSX,
					MENU_POSY + (i - page_start)*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2, // magic math?
					MENU_FONT,
					menu_list[i]);
	}
}

void menu_set_cursor(uint8_t cursor_pos) { // cursor_pos is the ABSOLUTE item position (1..MAINMENU_MAXITEMS)
	static uint8_t last_cursor_pos = 0; // 0 = none drawn yet; also absolute

	uint8_t page = (cursor_pos - 1) / MAINMENU_LINESPERPAGE;
	uint8_t row  = (cursor_pos - 1) % MAINMENU_LINESPERPAGE; // 0-based row on the current page

    // Erase the old cursor rectangle, but only if it's still on the same page.
    // A page change is already handled by a full menu_printlist() redraw.
    if (last_cursor_pos != 0 && last_cursor_pos != cursor_pos) {
		uint8_t last_page = (last_cursor_pos - 1) / MAINMENU_LINESPERPAGE;
		uint8_t last_row  = (last_cursor_pos - 1) % MAINMENU_LINESPERPAGE;
		if (last_page == page) {
			qp_rect(my_display,
			        MENU_POSX,
			        MENU_POSY + last_row*MENU_LINE_HEIGHT,
			        319,
			        MENU_POSY + (last_row+1)*MENU_LINE_HEIGHT,
			        MENU_COLOR_BACKGROUND,
			        true
			);
			qp_drawtext_recolor(my_display,
			                    MENU_POSX,
			                    MENU_POSY + last_row*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2, // magic math?
			                    MENU_FONT,
			                    menu_list[last_cursor_pos-1],
			                    HSV_WHITE,
			                    MENU_COLOR_BACKGROUND
			                );
		}
    }
	// Draw new cursor rectangle
	qp_rect(my_display,
			MENU_POSX,
			MENU_POSY + row*MENU_LINE_HEIGHT,
			319,
			MENU_POSY + (row+1)*MENU_LINE_HEIGHT,
			MENU_CURSOR_OUTLINE,
			true
	);
	qp_drawtext_recolor(my_display,
						MENU_POSX,
						MENU_POSY + row*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2, // magic math?
						MENU_FONT,
						menu_list[cursor_pos-1],
						HSV_BLACK,
						MENU_CURSOR_OUTLINE
					);

	last_cursor_pos = cursor_pos;
}

void menu_action(void) {
    if (menu_list_ischangeable[menu_cursor]) {
        menu_state = SUB_MENU;
		qp_drawimage_recolor(my_display,
							0, (239 - ico16_arrow_left->height),
							ico16_arrow_left,  HSV_BLACK, HSV_WHITE);
		qp_drawimage_recolor(my_display,
							(239 - ico16_arrow_right->width), (239 - ico16_arrow_right->height),
							ico16_arrow_right, HSV_BLACK, HSV_WHITE);
    }
    switch (menu_cursor) {
        case MENU_ACTIVATELAYER:
			action_activelayer();
            break;
        case MENU_ANIMATION:
			action_animation();
            break;
        case MENU_DISPLAYTIMEOUT:
			action_displaytimeout();
            break;
        case MENU_DISPLAYBRIGHTNESS:
			action_displaybrightness();
            break;
        case MENU_DISPLAYROTATION:
			action_displayrotation();
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
			action_aboutbuildbox();
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
        case MENU_BREAKOUT:
			action_breakout();
			break;
        default:
            break;
    }
}

void action_activelayer(void) {
	menu_state = SUB_MENU;
}

void action_animation(void) {
	menu_state = SUB_MENU;
}

void action_displaytimeout(void) {
	menu_state = SUB_MENU;
}

void action_displaybrightness(void) {
	menu_state = SUB_MENU;
}

void action_displayrotation(void) {
	menu_state = SUB_MENU;
}

void action_aboutbuildbox(void) {
	qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_COLOR_BACKGROUND, true); // Clear screen
	qp_drawtext(my_display, 0, MENU_FONT_HEIGHT*1, MENU_FONT, "      BuildBox     ");
	qp_drawtext(my_display, 0, MENU_FONT_HEIGHT*2, MENU_FONT, "a multi-function macropad");
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

void action_breakout(void) {
	menu_state  = NOT_IN_MENU;
	menu_cursor = MAINMENU_1STLINE_POS;
	accumulator = 0;
	breakout_start();
}

// Maximum matrix scan frequency: 4600-4700
void action_debug(void) {
    // qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_COLOR_BACKGROUND, true); // Clear screen
	// qp_drawtext(my_display, 0, 0, robotobold25, "DATAS FOR NERD");
}

// void eeprom_update_custom(void) {
// 	eeprom_update_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
// }

#endif // defined(QUANTUM_PAINTER_ENABLE)