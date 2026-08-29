#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp_menu.h"

#include "features/eeprom_custom.h"
#include "features/knob_custom.h"
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
uint8_t menu_cursor = MENU_1STLINE_POS;

static void menu_get_value_string(uint8_t item_pos, char *buf, size_t buflen);
static void menu_truncate_to_width(char *str, painter_font_handle_t font, uint16_t max_width);

void menu_init(void) {
	menu_state = MAIN_MENU;
	accumulator = 0; // clear this to avoid "weird cursor jump"
	qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_BACKGROUND, true); // Clear screen
	qp_roundrect(my_display,
	             MENU_POSX,
				 MENU_TITLE_POSY - MENU_FONT_HEIGHT/2 -3, // Refine
	             ST7789_WIDTH - MENU_POSX,
				 MENU_TITLE_POSY + MENU_FONT_HEIGHT/2,
	             MENU_TITLE_BG, true,
	             5, true, true); // Title background
	qp_drawtext_recolor_center(my_display,
							   ST7789_WIDTH/2,
							   MENU_TITLE_POSY,
							   MENU_FONT,
							   "SETTINGS",
							   MENU_TITLE_COLOR,
							   MENU_TITLE_BG); // Menu title
	qp_drawimage(my_display,
				ST7789_WIDTH/2 + qp_textwidth(thintel32, "SETTINGS")/2 + 10,
				MENU_TITLE_POSY - MENU_FONT_HEIGHT/2,
				ico18_heart); // decorative icon after the title
    qp_line(my_display, 10, 208, 310, 208, HSV_WHITE);
	qp_drawtext_recolor_center(my_display,
							   ST7789_WIDTH/2,
							   225,
							   MENU_FONT,
							   FW_VERSION,
							   HSV_WHITE,
							   HSV_BLACK); // Version number
	menu_printlist();               // Print the menu list and sidebar (value)
	menu_set_cursor(menu_cursor);   // Set the cursor
	qp_flush(my_display);
}

void menu_exit(void) {
    menu_state  = NOT_IN_MENU;
	accumulator = 0;

	menu_cursor = MENU_1STLINE_POS; // ignore cursor's latest position, reset to 1st menu line
	eeprom_update_custom(); // update all custom EEPROM values (if necessary)
    ui_refresh();
}

void menu_submenu_exit(void) { // Return from Sub Menu to Main Menu without a full-screen redraw
	menu_state = MAIN_MENU;
	accumulator = 0;
	// Redraw the value back in its normal (non-active) color
	uint8_t row = (menu_cursor - 1) % MENU_LINESPERPAGE;
	menu_render_sidebar(menu_cursor, row);
	// menu_set_cursor(menu_cursor);
	qp_flush(my_display);
}

static void menu_render_pagination(void) {
	uint8_t page      = (menu_cursor - 1) / MENU_LINESPERPAGE;
	uint8_t last_page = (MENU_MAXITEMS - 1) / MENU_LINESPERPAGE;

	qp_rect(my_display,
			MENU_PAGINATION_ARROW_POSX,
			MENU_PAGINATION_UP_POSY,
			MENU_PAGINATION_ARROW_POSX + MENU_PAGINATION_ARROW_WIDTH - 1,
			MENU_PAGINATION_UP_POSY + MENU_PAGINATION_ARROW_HEIGHT - 1,
			MENU_BACKGROUND, true);
	if (page > 0) {
		qp_drawimage_recolor(my_display,
							MENU_PAGINATION_ARROW_POSX,
							MENU_PAGINATION_UP_POSY,
							ico16_arrow_up, GLOBAL_THEME_COLOR, MENU_BACKGROUND);
	}

	qp_rect(my_display,
			MENU_PAGINATION_ARROW_POSX,
			MENU_PAGINATION_DOWN_POSY,
			MENU_PAGINATION_ARROW_POSX + MENU_PAGINATION_ARROW_WIDTH - 1,
			MENU_PAGINATION_DOWN_POSY + MENU_PAGINATION_ARROW_HEIGHT - 1,
			MENU_BACKGROUND, true);
	if (page < last_page) {
		qp_drawimage_recolor(my_display,
							MENU_PAGINATION_ARROW_POSX,
							MENU_PAGINATION_DOWN_POSY,
							ico16_arrow_down, GLOBAL_THEME_COLOR, MENU_BACKGROUND);
	}
}

void menu_printlist(void) { // Print the menu list, total MENU_LINESPERPAGE lines
	// Clear the old list + sidebar
	qp_rect(my_display,
	        MENU_POSX, MENU_POSY,
	        MENU_WIDTH, MENU_POSY + MENU_LINESPERPAGE * MENU_LINE_HEIGHT,
			MENU_BACKGROUND,
			true);
	// Print the menu-page that contains menu_cursor (handles any item count / page size)
	uint8_t page_start = ((menu_cursor - 1) / MENU_LINESPERPAGE) * MENU_LINESPERPAGE;
	uint8_t page_end   = page_start + MENU_LINESPERPAGE;
	if (page_end > MENU_MAXITEMS) page_end = MENU_MAXITEMS;

	for (uint8_t i = page_start; i < page_end; i++) {
		// Menu label
		qp_drawtext(my_display,
					MENU_POSX + MENU_CURSOR_ICON_WIDTH + 10,
					MENU_POSY + (i - page_start)*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2, // magic math?
					MENU_FONT,
					menu_label_list[i]);
		// Its value in sidebar
		menu_render_sidebar(i + 1, i - page_start); // item_pos is 1-based; row is 0-based on this page
	}
	menu_render_pagination();
}

void menu_set_cursor(uint8_t cursor_pos) { // cursor_pos is the ABSOLUTE item position (1..MENU_MAXITEMS)
	static uint8_t last_cursor_pos = 0; // 0 = none drawn yet; also absolute

	uint8_t page = (cursor_pos - 1) / MENU_LINESPERPAGE;
	uint8_t row  = (cursor_pos - 1) % MENU_LINESPERPAGE; // 0-based row on the current page

    // Erase the old cursor icon, but only if it's still on the same page.
    // A page change is already handled by a full menu_printlist() redraw.
    if (last_cursor_pos != 0 && last_cursor_pos != cursor_pos) {
		uint8_t last_page = (last_cursor_pos - 1) / MENU_LINESPERPAGE;
		uint8_t last_row  = (last_cursor_pos - 1) % MENU_LINESPERPAGE;
		if (last_page == page) {
			qp_rect(my_display,
			        MENU_POSX,
			        MENU_POSY + last_row*MENU_LINE_HEIGHT,
			        MENU_POSX + MENU_CURSOR_ICON_WIDTH - 1,
			        MENU_POSY + (last_row+1)*MENU_LINE_HEIGHT,
			        MENU_BACKGROUND,
			        true
				);
		}
    }
	// Draw new cursor icon
	qp_drawimage_recolor(my_display,
						MENU_POSX,
						MENU_POSY + row*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_CURSOR_ICON_HEIGHT)/2,
						ico16_arrow_right,
						MENU_CURSOR_COLOR,
						MENU_BACKGROUND
					);

	last_cursor_pos = cursor_pos;
}

// Fills 'buf' with the current value of menu item 'item_pos' (1-based), or leaves it empty if that item has none
static void menu_get_value_string(uint8_t item_pos, char *buf, size_t buflen) {
	buf[0] = '\0';
	switch (item_pos) {
		case MENU_DISPLAY_BRIGHTNESS:
			snprintf(buf, buflen, "%d%%", eepdata.display_brightness*10);
			break;
		case MENU_RGB_BRIGHTNESS:
			if (rgb_matrix_is_enabled() || (rgb_matrix_get_val() == 0)) {
				snprintf(buf, buflen, "%d%%", rgb_matrix_get_val()*5/12);
			} else {
				snprintf(buf, buflen, "RGB OFF");
			}
			break;
		case MENU_RGB_MODE:
			if (!rgb_matrix_is_enabled()) {
				snprintf(buf, buflen, "RGB OFF");
			} else {
				snprintf(buf, buflen, "MODE #%d", rgb_matrix_get_mode());
			}
			break;
		case MENU_KNOB_RGB:
			switch (eepdata.knob_effect) {
				case KNOB_EFFECT_DEFAULT: snprintf(buf, buflen, "DEFAULT"); break;
				case KNOB_EFFECT_LAYER:   snprintf(buf, buflen, "LAYER");   break;
				default:                  snprintf(buf, buflen, "OFF");    break;
			}
			break;
		case MENU_ANIMATION:
			snprintf(buf, buflen, "%s", eepdata.display_bootanim ? "ON" : "OFF");
			break;
		case MENU_DISPLAYTIMEOUT:
			if (eepdata.display_timeout >= DISPLAY_TIMEOUT_NEVER)
				snprintf(buf, buflen, "NEVER");
			else
				snprintf(buf, buflen, "%ds", eepdata.display_timeout);
			break;
		case MENU_KNOB_FUNC:
			snprintf(buf, buflen, "%s", knob_func_menu_text[eepdata.knob_func < KNOB_FUNC_COUNT ? eepdata.knob_func : KNOB_FUNC_CUSTOM]);
			break;
		default:
			break; // no value to show for this item
	}
}

// Chops characters off the end of 'str' until it fits within 'max_width' pixels when drawn in 'font'
static void menu_truncate_to_width(char *str, painter_font_handle_t font, uint16_t max_width) {
	uint8_t len = 0;
	while (str[len] != '\0') len++;
	while (len > 0 && qp_textwidth(font, str) > max_width) {
		str[--len] = '\0';
	}
}

// Render the value of menu item 'item_pos' (1-based) on page-relative 'row' (0-based), in the sidebar
void menu_render_sidebar(uint8_t item_pos, uint8_t row) {
	char value_str[16];
	menu_get_value_string(item_pos, value_str, sizeof(value_str));
	menu_truncate_to_width(value_str, MENU_FONT, MENU_SIDEBAR_MAX_TEXTWIDTH);

	// Clear the text cell first: a shorter string wouldn't otherwise overwrite
	// the tail of whatever longer string was previously drawn here.
	qp_rect(my_display,
	        MENU_SIDEBAR_TEXT_POSX, MENU_POSY + row*MENU_LINE_HEIGHT,
	        319, MENU_POSY + (row+1)*MENU_LINE_HEIGHT,
	        MENU_BACKGROUND, true);

	if (value_str[0] != '\0') {
		bool is_active = (menu_state == SUB_MENU && item_pos == menu_cursor);
		if (is_active) {
			qp_drawtext_recolor(my_display,
			                    MENU_SIDEBAR_TEXT_POSX,
			                    MENU_POSY + row*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2,
			                    MENU_FONT, value_str,
			                    GLOBAL_THEME_COLOR, MENU_BACKGROUND);
		} else {
			qp_drawtext_recolor(my_display,
			                    MENU_SIDEBAR_TEXT_POSX,
			                    MENU_POSY + row*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2,
			                    MENU_FONT, value_str,
			                    HSV_WHITE, MENU_BACKGROUND);
		}
	}
}

void menu_action(void) {
	if (menu_label_list_ischangeable[menu_cursor]) {
		menu_state = SUB_MENU;
		uint8_t row = (menu_cursor - 1) % MENU_LINESPERPAGE;
		menu_render_sidebar(menu_cursor, row); // redraw the value in the active (SUB_MENU) color
	}
	switch (menu_cursor) {
		case MENU_DISPLAY_BRIGHTNESS:
		case MENU_RGB_BRIGHTNESS:
		case MENU_RGB_MODE:
		case MENU_KNOB_RGB:
		case MENU_ANIMATION:
		case MENU_DISPLAYTIMEOUT:
		case MENU_KNOB_FUNC:
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
		case MENU_ABOUT:
			action_aboutbuildbox();
			break;
		default:
			break;
	}
	
}

void action_aboutbuildbox(void) {
	qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_BACKGROUND, true); // Clear screen
	qp_drawtext(my_display, 0, MENU_FONT_HEIGHT*1, MENU_FONT, "      BUILDBOX     ");
	qp_drawtext(my_display, 0, MENU_FONT_HEIGHT*2, MENU_FONT, "A MULTI-FUNCTION MACROPAD");
	// Add QR code & web link here
}

void action_resettodfu(void) {
	// rgb_matrix_set_color_all(RGB_BLACK);
	reset_keyboard();
}

void action_breakout(void) {
	menu_state  = NOT_IN_MENU;
	menu_cursor = MENU_1STLINE_POS;
	accumulator = 0;
	breakout_open();
}

void action_debug(void) {
	char buf[32];
	// qp_rect(my_display, 150, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_BACKGROUND, true); // Clear screen
	// qp_drawtext(my_display, 150, font_oled->line_height*0, font_oled, "EEPROM DEBUG");
	snprintf(buf, sizeof(buf), "layer:%d anim:%d",     eepdata.active_layer, eepdata.display_bootanim);
	qp_drawtext(my_display, 180, font_oled->line_height*1, font_oled, buf);
	snprintf(buf, sizeof(buf), "timeout:%d bright:%d", eepdata.display_timeout, eepdata.display_brightness);
	qp_drawtext(my_display, 180, font_oled->line_height*2, font_oled, buf);
	snprintf(buf, sizeof(buf), "rot:%d lly:%d llf:%d", eepdata.knob_effect, eepdata.lighting_layers, eepdata.lighting_flags);
	qp_drawtext(my_display, 180, font_oled->line_height*3, font_oled, buf);
	snprintf(buf, sizeof(buf), "hue: %d %d %d %d",     eepdata.layer_hue[0], eepdata.layer_hue[1], eepdata.layer_hue[2], eepdata.layer_hue[3]);
	qp_drawtext(my_display, 180, font_oled->line_height*4, font_oled, buf);
	snprintf(buf, sizeof(buf), "sat: %d %d %d %d",     eepdata.layer_sat[0], eepdata.layer_sat[1], eepdata.layer_sat[2], eepdata.layer_sat[3]);
	qp_drawtext(my_display, 180, font_oled->line_height*5, font_oled, buf);
	snprintf(buf, sizeof(buf), "knob:%d chk:%d",       eepdata.knob_func, eepdata.checksum);
	qp_drawtext(my_display, 180, font_oled->line_height*6, font_oled, buf);
	qp_flush(my_display);
}

// void action_factoryreset(void) {
// 	clear_keyboard();   // release all pressed keys if available
// 	eeprom_update_block(&eepdata_default, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
// 	eeconfig_disable();
// 	soft_reset_keyboard();
// }


void eeprom_update_custom(void) {
	eeprom_update_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
}

#endif // defined(QUANTUM_PAINTER_ENABLE)