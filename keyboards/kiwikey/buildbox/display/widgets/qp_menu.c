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
#include "display/widgets/qp_widget_screensaver.h"
#include "display/widgets/tutorial.h"
// #include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
// #include "display/widgets/qp_widget_knob.h"

extern painter_device_t my_display;
// extern bool    lcdoff_flag;
// extern bool    rgboff_flag;

uint8_t menu_state         = NOT_IN_MENU;
uint8_t menu_cursor        = MENU_1STLINE_POS;
uint8_t dial_menu_cursor   = MENU_1STLINE_POS;
uint8_t layers_menu_cursor = MENU_1STLINE_POS;

// MENU_DEBUG doesn't set menu_state to SUB_MENU (it's ischangeable=false), so
// without this, menu_state sits at MAIN_MENU the whole time action_debug()'s
// output is on screen - encoder rotation was being read as list navigation
// (process_encoder_rotate()) and redrawing the menu list right over it.
static bool debug_screen_active = false;

// MENU_BOOTTODFU is ischangeable=false too, so pressing Button 2 on it calls
// menu_action() -> action_resettodfu() again on every press (same generic
// MAIN_MENU routing debug/breakout/tutorial already rely on) - this just
// tracks whether the confirmation screen has already been shown once, so the
// first press shows it and the second one actually resets.
static bool dfu_confirm_active = false;

// Tracks how the "DIAL SETTINGS" sub-page was entered, so dial_menu_exit()'s
// Button 1 knows where "back" means: from the main list (MENU_DIAL_SETTINGS,
// via dial_menu_action()), it should return to the main list; from the 3s-hold
// shortcut on the idle screen (see housekeeping_task_display(), qp_graphics.c),
// there's no main list underneath it, so it should close the settings menu
// entirely instead, same as Button 1 on the main list itself.
static bool dial_menu_from_shortcut = false;

static void menu_get_value_string(uint8_t item_pos, char *buf, size_t buflen);
static void menu_truncate_to_width(char *str, painter_font_handle_t font, uint16_t max_width);
static void dial_menu_get_value_string(uint8_t item_pos, char *buf, size_t buflen);

// Shared chrome: title bar (with the gear icon) + bottom Exit/OK hint row.
// Used by both the main "SETTINGS" screen and the "DIAL SETTINGS" sub-page.
static void menu_draw_chrome(const char *title) {
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
							   title,
							   MENU_TITLE_COLOR,
							   MENU_TITLE_BG); // Menu title
	qp_drawimage(my_display,
				ST7789_WIDTH/2 - qp_textwidth(MENU_FONT, title)/2 - ico22_gear->width - 10,
				MENU_TITLE_POSY - ico22_gear->height/2 -1,
				ico22_gear); // decorative icon after the title
    qp_line(my_display, 10, 208, 310, 208, HSV_WHITE);
	qp_drawtext_recolor_center(my_display,
							   ST7789_WIDTH/2,
							   225,
							   MENU_FONT,
							   FW_VERSION,
							   HSV_WHITE,
							   HSV_BLACK); // Version number

	uint8_t dot_radius = 10;
	qp_circle(my_display, 10 + dot_radius, 225, dot_radius, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor(my_display, 10 + dot_radius*2 + 6, 232 - MENU_FONT_HEIGHT/2, font_oled, "Exit", HSV_WHITE, HSV_BLACK);

	uint16_t ok_width = qp_textwidth(font_oled, "OK");
	qp_drawtext_recolor(my_display, 310 - dot_radius*2 - 4 - ok_width, 232 - MENU_FONT_HEIGHT/2, font_oled, "OK", HSV_WHITE, HSV_BLACK);
	qp_circle(my_display, 310 - dot_radius, 225, dot_radius, GLOBAL_THEME_COLOR, true);
}

// Draws the whole main "SETTINGS" screen (chrome + list + cursor) - used both
// on first entry (menu_init()) and when returning from the "DIAL SETTINGS"
// sub-page (dial_menu_exit()).
static void main_menu_render(void) {
	menu_draw_chrome("SETTINGS");
	menu_printlist();             // Print the menu list and sidebar (value)
	menu_set_cursor(menu_cursor); // Set the cursor
}

void menu_init(void) {
	menu_state  = MAIN_MENU;
	accumulator = 0; // clear this to avoid "weird cursor jump"
	main_menu_render();
	qp_flush(my_display);
}

void menu_exit(void) {
    menu_state  = NOT_IN_MENU;
	accumulator = 0;
	debug_screen_active = false;
	dfu_confirm_active  = false;

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
					MENU_POSX + MENU_CURSOR_ICON_WIDTH + 5,
					MENU_POSY + (i - page_start)*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2, // magic math?
					MENU_FONT,
					menu_label_list[i]);
		// Its value in sidebar
		menu_render_sidebar(i + 1, i - page_start); // item_pos is 1-based; row is 0-based on this page
	}
	menu_render_pagination();

	// Update menu page number: [current page no]/[total pages], eg.: "1/3"
	uint8_t page = page_start / MENU_LINESPERPAGE;
	char    buf[8];
	snprintf(buf, sizeof(buf), " %d/%d", page + 1, (MENU_MAXITEMS - 1) / MENU_LINESPERPAGE + 1); // Data is counted from 0, so need to +1
	qp_drawtext_recolor_center(my_display,
								ST7789_WIDTH-40,
								MENU_TITLE_POSY,
								MENU_FONT,
								buf,
								MENU_TITLE_COLOR,
								MENU_TITLE_BG);
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

// Print the "DIAL SETTINGS" sub-page's 3 lines and their sidebar values. Always
// fits on one screen (DIAL_MENU_MAXITEMS < MENU_LINESPERPAGE), so unlike
// menu_printlist() there's no pagination to handle.
static void dial_menu_printlist(void) {
	qp_rect(my_display,
	        MENU_POSX, MENU_POSY,
	        MENU_WIDTH, MENU_POSY + DIAL_MENU_MAXITEMS * MENU_LINE_HEIGHT,
			MENU_BACKGROUND,
			true);
	for (uint8_t i = 0; i < DIAL_MENU_MAXITEMS; i++) {
		qp_drawtext(my_display,
					MENU_POSX + MENU_CURSOR_ICON_WIDTH + 5,
					MENU_POSY + i*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2,
					MENU_FONT,
					dial_menu_label_list[i]);
		dial_menu_render_sidebar(i + 1);
	}
}

void dial_menu_set_cursor(uint8_t cursor_pos) { // cursor_pos is ABSOLUTE (1..DIAL_MENU_MAXITEMS), single page only
	static uint8_t last_cursor_pos = 0; // 0 = none drawn yet

	uint8_t row = cursor_pos - 1;

	if (last_cursor_pos != 0 && last_cursor_pos != cursor_pos) {
		uint8_t last_row = last_cursor_pos - 1;
		qp_rect(my_display,
		        MENU_POSX,
		        MENU_POSY + last_row*MENU_LINE_HEIGHT,
		        MENU_POSX + MENU_CURSOR_ICON_WIDTH - 1,
		        MENU_POSY + (last_row+1)*MENU_LINE_HEIGHT,
		        MENU_BACKGROUND,
		        true
			);
	}
	qp_drawimage_recolor(my_display,
						MENU_POSX,
						MENU_POSY + row*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_CURSOR_ICON_HEIGHT)/2,
						ico16_arrow_right,
						MENU_CURSOR_COLOR,
						MENU_BACKGROUND
					);

	last_cursor_pos = cursor_pos;
}

void dial_menu_open(bool from_shortcut) { // Button 2 on MENU_DIAL_SETTINGS, or the idle-screen 3s-hold shortcut
	dial_menu_from_shortcut = from_shortcut;
	menu_state              = DIAL_MENU;
	dial_menu_cursor        = MENU_1STLINE_POS;
	accumulator             = 0;

	menu_draw_chrome("DIAL SETTINGS");
	dial_menu_printlist();
	dial_menu_set_cursor(dial_menu_cursor);
	qp_flush(my_display);
}

void dial_menu_exit(void) { // Button 1 on the list
	if (dial_menu_from_shortcut) {
		// No main list underneath this one - Button 1 closes the whole settings
		// menu instead, same as Button 1 on the main list (menu_exit())
		menu_exit();
		return;
	}
	menu_state  = MAIN_MENU;
	accumulator = 0;
	main_menu_render();
	qp_flush(my_display);
}

void dial_menu_action(void) { // Button 2 on a DIAL SETTINGS item - same role as menu_action()
	if (dial_menu_label_list_ischangeable[dial_menu_cursor]) {
		menu_state = DIAL_SUB_MENU;
		dial_menu_render_sidebar(dial_menu_cursor); // redraw the value in the active (DIAL_SUB_MENU) color
	}
	// Every item on this page is a changeable value (no "trigger immediately" items here)
}

void dial_menu_submenu_exit(void) { // Return from editing an item back to the DIAL SETTINGS list
	menu_state  = DIAL_MENU;
	accumulator = 0;
	dial_menu_render_sidebar(dial_menu_cursor); // redraw the value back in its normal (non-active) color
	qp_flush(my_display);
}

// Fills 'buf' with the current value of "DIAL SETTINGS" item 'item_pos' (1-based), or leaves it empty if that item has none
static void dial_menu_get_value_string(uint8_t item_pos, char *buf, size_t buflen) {
	buf[0] = '\0';
	switch (item_pos) {
		case DIAL_MENU_FUNCTION:
			snprintf(buf, buflen, "%s", knob_func_long_text[eepdata.knob_func < KNOB_FUNC_COUNT ? eepdata.knob_func : KNOB_FUNC_CUSTOM]);
			break;
		case DIAL_MENU_RGB_MODE:
			snprintf(buf, buflen, "%s", knob_effect_short_text[eepdata.knob_effect < KNOB_EFFECT_COUNT ? eepdata.knob_effect : KNOB_EFFECT_DEFAULT]);
			break;
		case DIAL_MENU_SENSITIVITY:
			snprintf(buf, buflen, "%s", knob_sensitivity_short_text[eepdata.knob_sensitivity < KNOB_SENSITIVITY_COUNT ? eepdata.knob_sensitivity : KNOB_SENSITIVITY_MEDIUM]);
			break;
		default:
			break; // no value to show for this item
	}
}

// Render the value of "DIAL SETTINGS" item 'item_pos' (1-based) in its sidebar - same role as menu_render_sidebar(),
// simplified since this page never paginates (row is always item_pos - 1)
void dial_menu_render_sidebar(uint8_t item_pos) {
	uint8_t row = item_pos - 1;

	qp_rect(my_display,
	        MENU_SIDEBAR_TEXT_POSX, MENU_POSY + row*MENU_LINE_HEIGHT,
	        319, MENU_POSY + (row+1)*MENU_LINE_HEIGHT,
	        MENU_BACKGROUND, true);

	char value_str[16];
	dial_menu_get_value_string(item_pos, value_str, sizeof(value_str));
	menu_truncate_to_width(value_str, MENU_FONT, MENU_SIDEBAR_MAX_TEXTWIDTH);

	if (value_str[0] != '\0') {
		bool is_active = (menu_state == DIAL_SUB_MENU && item_pos == dial_menu_cursor);
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

// Unlike every other menu page (plain text lines), the "LAYERS CONFIG"
// sub-page shows each layer as the same big rounded name box used elsewhere
// for the current layer (widget_layer_render_layername(), qp_widget_layer.c) -
// stacked LAYERS_MENU_ROW_HEIGHT apart so the boxes never collide, and
// indented past the cursor column like the other pages' text.
#define LAYERS_MENU_ROW_HEIGHT (WIDGET_LAYER_HEIGHT + 8)
#define LAYERS_MENU_BOX_POSX   (MENU_POSX + MENU_CURSOR_ICON_WIDTH + 5)

// Print the "LAYERS CONFIG" sub-page's lines, one box per layer_names[] entry
// (display/defines.h). Always fits on one screen (LAYERS_MENU_MAXITEMS <
// MENU_LINESPERPAGE), so unlike menu_printlist() there's no pagination to
// handle. No sidebar values yet - nothing here is wired to a real setting.
// widget_layer_render_layername() also draws each layer's fixed icon inside
// the box, next to the name text (qp_widget_layer.c).
static void layers_menu_printlist(void) {
	qp_rect(my_display,
	        MENU_POSX, MENU_POSY,
	        MENU_WIDTH, MENU_POSY + LAYERS_MENU_MAXITEMS * LAYERS_MENU_ROW_HEIGHT,
			MENU_BACKGROUND,
			true);
	for (uint8_t i = 0; i < LAYERS_MENU_MAXITEMS; i++) {
		widget_layer_render_layername(i, LAYERS_MENU_BOX_POSX, MENU_POSY + i*LAYERS_MENU_ROW_HEIGHT);
	}
}

void layers_menu_set_cursor(uint8_t cursor_pos) { // cursor_pos is ABSOLUTE (1..LAYERS_MENU_MAXITEMS), single page only
	static uint8_t last_cursor_pos = 0; // 0 = none drawn yet

	uint8_t row = cursor_pos - 1;

	if (last_cursor_pos != 0 && last_cursor_pos != cursor_pos) {
		uint8_t last_row = last_cursor_pos - 1;
		qp_rect(my_display,
		        MENU_POSX,
		        MENU_POSY + last_row*LAYERS_MENU_ROW_HEIGHT,
		        MENU_POSX + MENU_CURSOR_ICON_WIDTH - 1,
		        MENU_POSY + (last_row+1)*LAYERS_MENU_ROW_HEIGHT,
		        MENU_BACKGROUND,
		        true
			);
	}
	qp_drawimage_recolor(my_display,
						MENU_POSX,
						MENU_POSY + row*LAYERS_MENU_ROW_HEIGHT + (WIDGET_LAYER_HEIGHT - MENU_CURSOR_ICON_HEIGHT)/2,
						ico16_arrow_right,
						MENU_CURSOR_COLOR,
						MENU_BACKGROUND
					);

	last_cursor_pos = cursor_pos;
}

void layers_menu_open(void) { // Button 2 on MENU_LAYERS_CONFIG
	menu_state         = LAYERS_MENU;
	layers_menu_cursor = MENU_1STLINE_POS;
	accumulator        = 0;

	menu_draw_chrome("LAYERS CONFIG");
	layers_menu_printlist();
	layers_menu_set_cursor(layers_menu_cursor);
	qp_flush(my_display);
}

void layers_menu_exit(void) { // Button 1: back to the main "SETTINGS" list
	menu_state  = MAIN_MENU;
	accumulator = 0;
	main_menu_render();
	qp_flush(my_display);
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
		case MENU_INTROANIM:
			snprintf(buf, buflen, "%s", eepdata.display_bootanim ? "ON" : "OFF");
			break;
		case MENU_DISPLAYTIMEOUT: {
			uint8_t idx = eepdata.display_timeout < DISPLAY_TIMEOUT_COUNT ? eepdata.display_timeout : DISPLAY_TIMEOUT_NEVER_INDEX;
			snprintf(buf, buflen, "%s", display_timeout_text[idx]);
			break;
		}
		case MENU_SCREENSAVER: {
			uint8_t idx = eepdata.screensaver_effect < SCREEN_SAVER_MAXITEMS ? eepdata.screensaver_effect : 0;
			snprintf(buf, buflen, "%s", screen_saver_effect_list[idx]);
			break;
		}
		// MENU_THEME_COLOR is not handled here - menu_render_sidebar() draws its
		// preset name directly instead of going through this text path at all.
		// MENU_DIAL_SETTINGS/MENU_LAYERS_CONFIG aren't handled here either - they
		// open their own sub-page instead of showing an in-place value.
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
	// Clear the cell first: a shorter string (or the swatch below, narrower
	// than the cell) wouldn't otherwise overwrite whatever was drawn here before.
	qp_rect(my_display,
	        MENU_SIDEBAR_TEXT_POSX, MENU_POSY + row*MENU_LINE_HEIGHT,
	        319, MENU_POSY + (row+1)*MENU_LINE_HEIGHT,
	        MENU_BACKGROUND, true);

	if (item_pos == MENU_THEME_COLOR) {
		// Named preset (theme_color_presets[], display/defines.h), drawn in its
		// own color - the color itself is the content here, so unlike the
		// generic text branch below it doesn't swap to white when inactive.
		// process_encoder_rotate() calls menu_render_sidebar() on every knob
		// tick while this item's SUB_MENU is open, so this updates live.
		uint8_t index = theme_color_preset_index(eepdata.theme_hue);
		char    value_str[16];
		snprintf(value_str, sizeof(value_str), "%s", theme_color_presets[index].name);
		menu_truncate_to_width(value_str, MENU_FONT, MENU_SIDEBAR_MAX_TEXTWIDTH);

		qp_drawtext_recolor(my_display,
		                    MENU_SIDEBAR_TEXT_POSX,
		                    MENU_POSY + row*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2,
		                    MENU_FONT, value_str,
		                    theme_color_presets[index].hue, 255, 255,
		                    MENU_BACKGROUND);
		return;
	}

	char value_str[16];
	menu_get_value_string(item_pos, value_str, sizeof(value_str));
	menu_truncate_to_width(value_str, MENU_FONT, MENU_SIDEBAR_MAX_TEXTWIDTH);

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
		case MENU_INTROANIM:
		case MENU_DISPLAYTIMEOUT:
		case MENU_THEME_COLOR:
			break;
		case MENU_DIAL_SETTINGS:
			dial_menu_open(false); // via the main list
			break;
		case MENU_LAYERS_CONFIG:
			layers_menu_open();
			break;
		case MENU_BOOTTODFU:
			action_resettodfu();
			break;
		// case MENU_DEBUG:
		// 	action_debug();
		// 	break;
		case MENU_BREAKOUT:
			action_breakout();
			break;
		case MENU_ABOUT:
			action_aboutbuildbox();
			break;
		case MENU_TUTORIAL:
			action_tutorial();
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
	if (!dfu_confirm_active) {
		dfu_confirm_active = true;

		qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_BACKGROUND, true); // Clear screen
		qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, TUTORIAL_TITLE_POSY, TUTORIAL_TITLE_FONT,
			"Enter Bootloader Mode",
			HSV_WHITE, MENU_BACKGROUND);
		qp_drawtext_recolor(my_display, 20, TUTORIAL_TITLE_POSY*3, TUTORIAL_BODY_FONT,
			"- An external drive will appear in",
			HSV_WHITE, MENU_BACKGROUND);
		qp_drawtext_recolor(my_display, 20, TUTORIAL_TITLE_POSY*4, TUTORIAL_BODY_FONT,
			"                     your computer",
			HSV_WHITE, MENU_BACKGROUND);
		qp_drawtext_recolor(my_display, 20, TUTORIAL_TITLE_POSY*6, TUTORIAL_BODY_FONT,
			"- Copy firmware file to it",
			HSV_WHITE, MENU_BACKGROUND);

		qp_circle(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
		qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Cancel", HSV_WHITE, MENU_BACKGROUND);
		qp_circle(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
		qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "OK", HSV_WHITE, MENU_BACKGROUND);
		qp_flush(my_display);
		return;
	}

	// rgb_matrix_set_color_all(RGB_BLACK);
	reset_keyboard();
}

void action_breakout(void) {
	menu_state  = NOT_IN_MENU;
	menu_cursor = MENU_1STLINE_POS;
	accumulator = 0;
	breakout_open();
}

void action_tutorial(void) {
	menu_state  = NOT_IN_MENU; // the tutorial owns the screen now, not the menu - same as action_breakout()
	menu_cursor = MENU_1STLINE_POS;
	accumulator = 0;
	tutorial_start();
}

bool debug_screen_is_active(void) {
	return debug_screen_active;
}

bool dfu_confirm_screen_is_active(void) {
	return dfu_confirm_active;
}

void action_debug(void) {
	debug_screen_active = true;
	char buf[40]; // longest line is "+ theme_color: [ 255, 255, 255 ]" (33 chars + null)
	uint8_t line = 0;
	qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, MENU_BACKGROUND, true); // Clear screen

	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16, "*** DEBUG ***");

	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16, "Display");
	snprintf(buf, sizeof(buf), "+ resolution: %d*%d px", DISPLAY_WIDTH, DISPLAY_HEIGHT);
	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16,
		#if defined(QUANTUM_PAINTER_ST7789_SPI_ENABLE)
		"+ driver: ST7789"
		#else
		"+ driver: ILI9341"
		#endif
	);

	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16, "EEPROM");
	snprintf(buf, sizeof(buf), "+ layer:%d anim:%d",     eepdata.active_layer, eepdata.display_bootanim);
	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ timeout:%d bright:%d", eepdata.display_timeout, eepdata.display_brightness);
	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ rot:%d lly:%d", eepdata.knob_effect, eepdata.lighting_layers);
	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ hue: %d %d %d %d",     eepdata.layer_hue[0], eepdata.layer_hue[1], eepdata.layer_hue[2], eepdata.layer_hue[3]);
	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ sat: %d %d %d %d",     eepdata.layer_sat[0], eepdata.layer_sat[1], eepdata.layer_sat[2], eepdata.layer_sat[3]);
	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ knob:%d chk:%d",       eepdata.knob_func, eepdata.checksum);
	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ theme_color: [ %d, 255, 255 ]", eepdata.theme_hue);
	qp_drawtext(my_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);

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