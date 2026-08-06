// TEMPORARY / DRAFT - keyboard-driven Menu navigation.
//
// Alternate to knob-rotate-to-move-cursor: while in the Menu, this lights up
// the grid key for each item on the current page (in GLOBAL_THEME_COLOR) plus
// two dedicated paging keys (in white), and pressing an item key jumps
// straight into it - from the Main Menu, or straight over from whatever
// Sub Menu is currently open. While in the Menu, this owns every LED: the
// active RGB Matrix effect, Lighting Layers and the knob ring effect are all
// suppressed so only the selection keys show anything.
//
// Wired in via: rules.mk (SRC), and qp_graphics.c's process_record_display()
// MAIN_MENU + SUB_MENU branches call menu_keyselect_process_record() from
// their `default:` case instead of just `return false`.

#include "menu_keyselect.h"

#include "display/defines.h"
#include "display/qp_graphics.h"
#include "display/widgets/qp_menu.h"

#if MENU_KEYSELECT_ENABLE // qp_menu.h - false makes this file compile to nothing, RGB Matrix and
                          // process_record_display() fall back to old (pre-keyselect) behavior

static uint8_t keyselect_page_start(void) { // 0-based index into menu_label_list for the current page
	uint8_t page = (menu_cursor - 1) / MENU_LINESPERPAGE;
	return page * MENU_LINESPERPAGE;
}

static uint8_t keyselect_slot_count(void) { // how many item slots are filled on the current page
	uint8_t page_end = keyselect_page_start() + MENU_LINESPERPAGE;
	if (page_end > MENU_MAXITEMS) page_end = MENU_MAXITEMS;
	return page_end - keyselect_page_start();
}

void menu_keyselect_highlight(uint8_t led_min, uint8_t led_max) {
	if (menu_state == NOT_IN_MENU) return; // leave the active RGB Matrix effect alone

	// While in the Menu (Main or Sub), this owns every LED for the frame: blank
	// everything first, then light only the selection keys on top. The caller
	// also returns false so nothing repaints over this afterwards (see below).
	for (uint8_t i = led_min; i < led_max; i++) {
		rgb_matrix_set_color(i, 0, 0, 0);
	}

	HSV item_hsv = {GLOBAL_THEME_COLOR};
	RGB item_rgb = hsv_to_rgb(item_hsv);
	uint8_t slot_count = keyselect_slot_count();

	for (uint8_t slot = 0; slot < MENU_KEYSELECT_ITEM_SLOTS && slot < slot_count; slot++) {
		if (slot < led_min || slot >= led_max) continue;
		rgb_matrix_set_color(slot, item_rgb.r, item_rgb.g, item_rgb.b);
	}

	uint8_t page      = (menu_cursor - 1) / MENU_LINESPERPAGE;
	uint8_t last_page = (MENU_MAXITEMS - 1) / MENU_LINESPERPAGE;
	HSV     nav_hsv   = {HSV_WHITE};
	RGB     nav_rgb   = hsv_to_rgb(nav_hsv);

	if (page > 0 && MENU_KEYSELECT_LED_PAGEPREV >= led_min && MENU_KEYSELECT_LED_PAGEPREV < led_max) {
		rgb_matrix_set_color(MENU_KEYSELECT_LED_PAGEPREV, nav_rgb.r, nav_rgb.g, nav_rgb.b);
	}
	if (page < last_page && MENU_KEYSELECT_LED_PAGENEXT >= led_min && MENU_KEYSELECT_LED_PAGENEXT < led_max) {
		rgb_matrix_set_color(MENU_KEYSELECT_LED_PAGENEXT, nav_rgb.r, nav_rgb.g, nav_rgb.b);
	}
}

// Grid keys only (row 0-3); LED index == row*MATRIX_COLS + col, matching keyboard.json's rgb_matrix layout order.
static uint8_t matrix_to_led(keyrecord_t *record) {
	return record->event.key.row * MATRIX_COLS + record->event.key.col;
}

bool menu_keyselect_process_record(keyrecord_t *record) {
	if ((menu_state != MAIN_MENU && menu_state != SUB_MENU) || !record->event.pressed) return false;
	if (record->event.key.row >= 4) return false; // Button 1/2 - not part of the grid, handled elsewhere

	uint8_t led          = matrix_to_led(record);
	bool    was_submenu  = (menu_state == SUB_MENU); // page/item lookups below use menu_cursor, untouched by exiting

	if (led == MENU_KEYSELECT_LED_PAGEPREV || led == MENU_KEYSELECT_LED_PAGENEXT) {
		bool    go_prev   = (led == MENU_KEYSELECT_LED_PAGEPREV);
		uint8_t page      = (menu_cursor - 1) / MENU_LINESPERPAGE;
		uint8_t last_page = (MENU_MAXITEMS - 1) / MENU_LINESPERPAGE;
		if ((go_prev && page == 0) || (!go_prev && page >= last_page)) return true;

		if (was_submenu) menu_submenu_exit(); // quit the open Sub Menu before paging away from it
		menu_cursor = (go_prev ? page - 1 : page + 1) * MENU_LINESPERPAGE + 1;
		menu_printlist();
		menu_set_cursor(menu_cursor);
		qp_flush(my_display);
		return true;
	}

	if (led < MENU_KEYSELECT_ITEM_SLOTS && led < keyselect_slot_count()) {
		if (was_submenu) menu_submenu_exit(); // quit the open Sub Menu before jumping to the pressed one
		menu_cursor = keyselect_page_start() + led + 1; // absolute, 1-based
		menu_set_cursor(menu_cursor);
		menu_action();
		qp_flush(my_display);
		return true;
	}

	return true; // unmapped/blank key while in the menu - swallow it, same as today
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
	menu_keyselect_highlight(led_min, led_max);
	// While in the Menu, menu_keyselect_highlight() already owns every LED for
	// this frame - returning false here skips Lighting Layers / the knob ring
	// effect in rgb_matrix_indicators_advanced_kb() (buildbox.c) so neither
	// repaints over it.
	return menu_state == NOT_IN_MENU;
}

#endif // MENU_KEYSELECT_ENABLE
