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
#include "display/widgets/qp_widget_knob.h"

static const theme_color_preset_t theme_color_presets[] = {
	{   0, "Red"     },
	{  21, "Orange"  },
	{  43, "Yellow"  },
	{  64, "Lime"    },
	{  85, "Green"   },
	{ 128, "Cyan"    },
	{ 149, "Sky Blue"},
	{ 170, "Blue"    },
	{ 191, "Purple"  },
	{ 213, "Magenta" },
};
_Static_assert(sizeof(theme_color_presets) / sizeof(theme_color_presets[0]) == (THEME_COLOR_PRESET_COUNT), "theme_color_presets must have exactly THEME_COLOR_PRESET_COUNT entries");

static const char * const screen_saver_effect_list[] = {
	"OFF",
	"Rain 1",
	"Rain 2",
	"Zzz...",
	"Starry"
};
_Static_assert(sizeof(screen_saver_effect_list) / sizeof(screen_saver_effect_list[0]) == (SCREEN_SAVER_MAXITEMS), "screen_saver_effect_list must have exactly SCREEN_SAVER_MAXITEMS entries");

// Sidebar text for "DIAL RGB MODE" (qp_menu.c's DIAL SETTINGS sub-page), indexed by eepdata.knob_effect.
static const char * const knob_effect_short_text[] = {
	"OFF",
	"RGB EFF",
	"LAYER",
};
_Static_assert(sizeof(knob_effect_short_text) / sizeof(knob_effect_short_text[0]) == (KNOB_EFFECT_COUNT), "knob_effect_short_text must have exactly KNOB_EFFECT_COUNT entries");

// Sidebar text for "DIAL FUNCTION" (qp_menu.c's DIAL SETTINGS sub-page), indexed by eepdata.knob_func.
static const char * const knob_func_long_text[] = {
	"H-SCROLL",
	"V-SCROLL",
	"VOLUME",
	"CUSTOM"
};
_Static_assert(sizeof(knob_func_long_text) / sizeof(knob_func_long_text[0]) == (KNOB_FUNC_COUNT), "knob_func_long_text must have exactly KNOB_FUNC_COUNT entries");

// Sidebar text for "DIAL SENSITIVITY" (qp_menu.c's DIAL SETTINGS sub-page), indexed by eepdata.knob_sensitivity.
static const char * const knob_sensitivity_short_text[] = {
	"LOW",
	"MEDIUM",
	"HIGH",
};
_Static_assert(sizeof(knob_sensitivity_short_text) / sizeof(knob_sensitivity_short_text[0]) == (KNOB_SENSITIVITY_COUNT), "knob_sensitivity_short_text must have exactly KNOB_SENSITIVITY_COUNT entries");

// Index into theme_color_presets[] matching `hue` exactly, or 0 if it isn't
// one of the presets (e.g. eepdata.theme_hue still at its EEPROM default).
static uint8_t theme_color_preset_index(uint8_t hue) {
	for (uint8_t i = 0; i < THEME_COLOR_PRESET_COUNT; i++) {
		if (theme_color_presets[i].hue == hue) return i;
	}
	return 0;
}


uint8_t menu_cursor        = MENU_1STLINE_POS;
uint8_t dial_menu_cursor   = MENU_1STLINE_POS;
uint8_t layers_menu_cursor = MENU_1STLINE_POS;

// Tracks how the "DIAL SETTINGS" sub-page was entered, so dial_menu_exit()'s
// Button 1 knows where "back" means: from the main list (DIAL SETTINGS item),
// it should return to the main list; from the 3s-hold shortcut on the idle
// screen (see housekeeping_task_display(), qp_graphics.c), there's no main list
// underneath it, so it should close the settings menu entirely instead, same
// as Button 1 on the main list itself.
static bool dial_menu_from_shortcut = false;

static void menu_truncate_to_width(char *str, painter_font_handle_t font, uint16_t max_width);

// Next/previous value of an index setting with `count` options, wrapping at both ends.
// Every eepdata field is range-checked by eeprom_custom_validate() (load + VIA), so
// `value` is always < count here.
static uint8_t cycle_index(uint8_t value, uint8_t count, bool clockwise) {
	return clockwise ? (value + 1) % count : (value + count - 1) % count;
}

/*** Main "SETTINGS" list - item callbacks ***/

static void value_lcd_brightness(char *buf, size_t len) { snprintf(buf, len, "%d%%", eepdata.display_brightness * 100 / BACKLIGHT_LEVELS); }
static void rotate_lcd_brightness(bool cw) {
	eepdata.display_brightness = cycle_index(eepdata.display_brightness - 1, BACKLIGHT_LEVELS, cw) + 1; // 1..BACKLIGHT_LEVELS
	backlight_level_noeeprom(eepdata.display_brightness); // eepdata is the one saved copy (eeprom_custom_save() on menu exit)
}

static void value_sleep(char *buf, size_t len) { snprintf(buf, len, "%s", display_timeout_text[eepdata.display_timeout]); }
static void rotate_sleep(bool cw) { eepdata.display_timeout = cycle_index(eepdata.display_timeout, DISPLAY_TIMEOUT_COUNT, cw); }

static void value_screensaver(char *buf, size_t len) { snprintf(buf, len, "%s", screen_saver_effect_list[eepdata.screensaver_effect]); }
static void rotate_screensaver(bool cw) { eepdata.screensaver_effect = cycle_index(eepdata.screensaver_effect, SCREEN_SAVER_MAXITEMS, cw); }

// Named preset (theme_color_presets[]), drawn in its own color - the color itself
// is the content here, so unlike other values it doesn't swap to white when inactive.
static void    value_theme_color(char *buf, size_t len) { snprintf(buf, len, "%s", theme_color_presets[theme_color_preset_index(eepdata.theme_hue)].name); }
static uint8_t hue_theme_color(void) { return theme_color_presets[theme_color_preset_index(eepdata.theme_hue)].hue; }
static void    rotate_theme_color(bool cw) {
	eepdata.theme_hue = theme_color_presets[cycle_index(theme_color_preset_index(eepdata.theme_hue), THEME_COLOR_PRESET_COUNT, cw)].hue;
}

// RGB settings live in QMK's own EEPROM block: changed live with the _noeeprom
// variants while turning, written once when the edit ends (commit_rgb_settings()).
static void value_rgb_brightness(char *buf, size_t len) {
	if (rgb_matrix_is_enabled() || rgb_matrix_get_val() == 0) {
		snprintf(buf, len, "%d%%", rgb_matrix_get_val() * 100 / RGB_MATRIX_MAXIMUM_BRIGHTNESS);
	} else {
		snprintf(buf, len, "RGB OFF");
	}
}
static void rotate_rgb_brightness(bool cw) { cw ? rgb_matrix_increase_val_noeeprom() : rgb_matrix_decrease_val_noeeprom(); }

static void value_rgb_mode(char *buf, size_t len) {
	if (!rgb_matrix_is_enabled()) {
		snprintf(buf, len, "RGB OFF");
	} else {
		snprintf(buf, len, "MODE #%d", rgb_matrix_get_mode());
	}
}
static void rotate_rgb_mode(bool cw) { cw ? rgb_matrix_step_noeeprom() : rgb_matrix_step_reverse_noeeprom(); }
static void commit_rgb_settings(void) { eeconfig_force_flush_rgb_matrix(); }

static void value_intro(char *buf, size_t len) { snprintf(buf, len, "%s", eepdata.display_bootanim ? "ON" : "OFF"); }
static void rotate_intro(bool cw) { eepdata.display_bootanim = !eepdata.display_bootanim; }

static void open_dial_settings(void) { dial_menu_open(false); } // via the main list, not the idle-screen shortcut

/*** Main "SETTINGS" list - one row per item, in display order. Reorder,
	add or remove rows here; nothing else needs to change.
	- on_rotate set: OK edits the value in place (UI_MODE_MENU_EDIT), the knob
	  changes it live, OK/Exit ends the edit (on_edit_done, if set, runs then).
	- on_select set: OK runs it instead (opens a sub-page or screen).
	- neither: not selectable - the cursor skips it (divider lines).
***/
typedef struct {
	const char *label;
	void    (*get_value)(char *buf, size_t len); // sidebar text - NULL = no value shown
	uint8_t (*value_hue)(void);                  // set = value always drawn in this hue instead of white/theme
	void    (*on_rotate)(bool clockwise);
	void    (*on_edit_done)(void);
	void    (*on_select)(void);
} menu_item_t;

static const menu_item_t menu_items[] = {
	{ .label = "LCD BRIGHTNESS", .get_value = value_lcd_brightness, .on_rotate = rotate_lcd_brightness },
	{ .label = "SLEEP",          .get_value = value_sleep,          .on_rotate = rotate_sleep },
	{ .label = "SCREEN SAVER",   .get_value = value_screensaver,    .on_rotate = rotate_screensaver },
	{ .label = "DIAL SETTINGS",  .on_select = open_dial_settings },
	{ .label = "THEME COLOR",    .get_value = value_theme_color, .value_hue = hue_theme_color, .on_rotate = rotate_theme_color },

	{ .label = "RGB BRIGHTNESS", .get_value = value_rgb_brightness, .on_rotate = rotate_rgb_brightness, .on_edit_done = commit_rgb_settings },
	{ .label = "RGB MODE",       .get_value = value_rgb_mode,       .on_rotate = rotate_rgb_mode,       .on_edit_done = commit_rgb_settings },
	{ .label = "LAYERS CONFIG",  .on_select = layers_menu_open },
	{ .label = "BUILDBOX INTRO", .get_value = value_intro,          .on_rotate = rotate_intro },
	{ .label = "   ------" }, // divider line
	{ .label = "ABOUT BUILDBOX", .on_select = action_aboutbuildbox },
	{ .label = "SECRET GAME",    .on_select = action_breakout },
	// { .label = "DEBUG",       .on_select = action_debug },
	{ .label = "BOOT TO DFU",    .on_select = action_resettodfu },
	{ .label = "QUICK TUTORIAL", .on_select = action_tutorial },
};
#define MENU_MAXITEMS ((uint8_t)(sizeof(menu_items) / sizeof(menu_items[0])))

static bool menu_item_is_selectable(uint8_t item_pos) { // item_pos is 1-based
	const menu_item_t *item = &menu_items[item_pos - 1];
	return item->on_rotate || item->on_select;
}

/*** "DIAL SETTINGS" sub-page - item callbacks + rows (every row is an in-place value) ***/

static void value_knob_func(char *buf, size_t len)        { snprintf(buf, len, "%s", knob_func_long_text[eepdata.knob_func]); }
static void rotate_knob_func(bool cw)                     { eepdata.knob_func = cycle_index(eepdata.knob_func, KNOB_FUNC_COUNT, cw); }
static void value_knob_effect(char *buf, size_t len)      { snprintf(buf, len, "%s", knob_effect_short_text[eepdata.knob_effect]); }
static void rotate_knob_effect(bool cw)                   { eepdata.knob_effect = cycle_index(eepdata.knob_effect, KNOB_EFFECT_COUNT, cw); }
static void value_knob_sensitivity(char *buf, size_t len) { snprintf(buf, len, "%s", knob_sensitivity_short_text[eepdata.knob_sensitivity]); }
static void rotate_knob_sensitivity(bool cw)              { eepdata.knob_sensitivity = cycle_index(eepdata.knob_sensitivity, KNOB_SENSITIVITY_COUNT, cw); } // CW: less -> more sensitive

typedef struct {
	const char *label;
	void (*get_value)(char *buf, size_t len);
	void (*on_rotate)(bool clockwise);
} dial_item_t;

static const dial_item_t dial_items[] = {
	{ "FUNCTION", value_knob_func,        rotate_knob_func },
	{ "RGB MODE", value_knob_effect,      rotate_knob_effect },
	{ "SPEED",    value_knob_sensitivity, rotate_knob_sensitivity },
};
#define DIAL_MENU_MAXITEMS ((uint8_t)(sizeof(dial_items) / sizeof(dial_items[0])))

// Shared chrome: title bar (with the gear icon) + bottom Exit/OK hint row.
// Used by both the main "SETTINGS" screen and the "DIAL SETTINGS" sub-page.
static void menu_draw_chrome(const char *title) {
	qp_rect(bb_display, 0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, MENU_BACKGROUND, true); // Clear screen
	bb_roundrect(bb_display,
	             MENU_POSX,
				 MENU_TITLE_POSY - MENU_FONT_HEIGHT/2 -3, // Refine
	             DISPLAY_WIDTH - MENU_POSX,
				 MENU_TITLE_POSY + MENU_FONT_HEIGHT/2,
	             MENU_TITLE_BG, true,
	             5, true, true); // Title background
	bb_drawtext_recolor_center(bb_display,
							   DISPLAY_WIDTH/2,
							   MENU_TITLE_POSY,
							   MENU_FONT,
							   title,
							   MENU_TITLE_COLOR,
							   MENU_TITLE_BG); // Menu title
	if (ico22_gear) { // NULL if it failed to load (qp_includes.c) - its size is read directly below
		qp_drawimage(bb_display,
					DISPLAY_WIDTH/2 - qp_textwidth(MENU_FONT, title)/2 - ico22_gear->width - 10,
					MENU_TITLE_POSY - ico22_gear->height/2 -1,
					ico22_gear); // decorative icon after the title
	}
	qp_line(bb_display, 10, 208, 310, 208, HSV_WHITE);
	bb_drawtext_recolor_center(bb_display,
							   DISPLAY_WIDTH/2,
							   225,
							   MENU_FONT,
							   FW_VERSION,
							   HSV_WHITE,
							   HSV_BLACK); // Version number

	uint8_t dot_radius = 10;
	qp_circle(bb_display, 10 + dot_radius, 225, dot_radius, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor(bb_display, 10 + dot_radius*2 + 6, 232 - MENU_FONT_HEIGHT/2, font_oled, "Exit", HSV_WHITE, HSV_BLACK);

	uint16_t ok_width = qp_textwidth(font_oled, "OK");
	qp_drawtext_recolor(bb_display, 310 - dot_radius*2 - 4 - ok_width, 232 - MENU_FONT_HEIGHT/2, font_oled, "OK", HSV_WHITE, HSV_BLACK);
	qp_circle(bb_display, 310 - dot_radius, 225, dot_radius, GLOBAL_THEME_COLOR, true);
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
	ui_set_mode(UI_MODE_MENU_LIST);
	main_menu_render();
	qp_flush(bb_display);
}

void menu_exit(void) {
	menu_cursor = MENU_1STLINE_POS; // ignore cursor's latest position, reset to 1st menu line
	ui_set_mode(UI_MODE_IDLE);      // saves eepdata (leaving the menu) and redraws the idle screen
}

void menu_submenu_exit(void) { // End an in-place edit, back to the main list without a full-screen redraw
	const menu_item_t *item = &menu_items[menu_cursor - 1];
	if (item->on_edit_done) item->on_edit_done();
	ui_set_mode(UI_MODE_MENU_LIST);
	// Redraw the value back in its normal (non-active) color
	uint8_t row = (menu_cursor - 1) % MENU_LINESPERPAGE;
	menu_render_sidebar(menu_cursor, row);
	// menu_set_cursor(menu_cursor);
	qp_flush(bb_display);
}

static void menu_render_pagination(void) {
	uint8_t page      = (menu_cursor - 1) / MENU_LINESPERPAGE;
	uint8_t last_page = (MENU_MAXITEMS - 1) / MENU_LINESPERPAGE;

	qp_rect(bb_display,
			MENU_PAGINATION_ARROW_POSX,
			MENU_PAGINATION_UP_POSY,
			MENU_PAGINATION_ARROW_POSX + MENU_PAGINATION_ARROW_WIDTH - 1,
			MENU_PAGINATION_UP_POSY + MENU_PAGINATION_ARROW_HEIGHT - 1,
			MENU_BACKGROUND, true);
	if (page > 0) {
		qp_drawimage_recolor(bb_display,
							MENU_PAGINATION_ARROW_POSX,
							MENU_PAGINATION_UP_POSY,
							ico16_arrow_up, GLOBAL_THEME_COLOR, MENU_BACKGROUND);
	}

	qp_rect(bb_display,
			MENU_PAGINATION_ARROW_POSX,
			MENU_PAGINATION_DOWN_POSY,
			MENU_PAGINATION_ARROW_POSX + MENU_PAGINATION_ARROW_WIDTH - 1,
			MENU_PAGINATION_DOWN_POSY + MENU_PAGINATION_ARROW_HEIGHT - 1,
			MENU_BACKGROUND, true);
	if (page < last_page) {
		qp_drawimage_recolor(bb_display,
							MENU_PAGINATION_ARROW_POSX,
							MENU_PAGINATION_DOWN_POSY,
							ico16_arrow_down, GLOBAL_THEME_COLOR, MENU_BACKGROUND);
	}
}

void menu_printlist(void) { // Print the menu list, total MENU_LINESPERPAGE lines
	// Clear the old list + sidebar
	qp_rect(bb_display,
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
		qp_drawtext(bb_display,
					MENU_POSX + MENU_CURSOR_ICON_WIDTH + 5,
					MENU_POSY + (i - page_start)*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2, // magic math?
					MENU_FONT,
					menu_items[i].label);
		// Its value in sidebar
		menu_render_sidebar(i + 1, i - page_start); // item_pos is 1-based; row is 0-based on this page
	}
	menu_render_pagination();

	// Update menu page number: [current page no]/[total pages], eg.: "1/3"
	uint8_t page = page_start / MENU_LINESPERPAGE;
	char    buf[8];
	snprintf(buf, sizeof(buf), " %d/%d", page + 1, (MENU_MAXITEMS - 1) / MENU_LINESPERPAGE + 1); // Data is counted from 0, so need to +1
	bb_drawtext_recolor_center(bb_display,
								DISPLAY_WIDTH-40,
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
			qp_rect(bb_display,
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
	qp_drawimage_recolor(bb_display,
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
	qp_rect(bb_display,
	        MENU_POSX, MENU_POSY,
	        MENU_WIDTH, MENU_POSY + DIAL_MENU_MAXITEMS * MENU_LINE_HEIGHT,
			MENU_BACKGROUND,
			true);
	for (uint8_t i = 0; i < DIAL_MENU_MAXITEMS; i++) {
		qp_drawtext(bb_display,
					MENU_POSX + MENU_CURSOR_ICON_WIDTH + 5,
					MENU_POSY + i*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2,
					MENU_FONT,
					dial_items[i].label);
		dial_menu_render_sidebar(i + 1);
	}
}

void dial_menu_set_cursor(uint8_t cursor_pos) { // cursor_pos is ABSOLUTE (1..DIAL_MENU_MAXITEMS), single page only
	static uint8_t last_cursor_pos = 0; // 0 = none drawn yet

	uint8_t row = cursor_pos - 1;

	if (last_cursor_pos != 0 && last_cursor_pos != cursor_pos) {
		uint8_t last_row = last_cursor_pos - 1;
		qp_rect(bb_display,
		        MENU_POSX,
		        MENU_POSY + last_row*MENU_LINE_HEIGHT,
		        MENU_POSX + MENU_CURSOR_ICON_WIDTH - 1,
		        MENU_POSY + (last_row+1)*MENU_LINE_HEIGHT,
		        MENU_BACKGROUND,
		        true
			);
	}
	qp_drawimage_recolor(bb_display,
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
	ui_set_mode(UI_MODE_DIAL_LIST);
	dial_menu_cursor        = MENU_1STLINE_POS;

	menu_draw_chrome("DIAL SETTINGS");
	dial_menu_printlist();
	dial_menu_set_cursor(dial_menu_cursor);
	qp_flush(bb_display);
}

void dial_menu_exit(void) { // Button 1 on the list
	if (dial_menu_from_shortcut) {
		// No main list underneath this one - Button 1 closes the whole settings
		// menu instead, same as Button 1 on the main list (menu_exit())
		menu_exit();
		return;
	}
	ui_set_mode(UI_MODE_MENU_LIST);
	main_menu_render();
	qp_flush(bb_display);
}

void dial_menu_action(void) { // Button 2 on a DIAL SETTINGS item - same role as menu_action()
	// Every item on this page is an in-place value (no "trigger immediately" items here)
	ui_set_mode(UI_MODE_DIAL_EDIT);
	dial_menu_render_sidebar(dial_menu_cursor); // redraw the value in the active (editing) color
}

void dial_menu_submenu_exit(void) { // Return from editing an item back to the DIAL SETTINGS list
	ui_set_mode(UI_MODE_DIAL_LIST);
	dial_menu_render_sidebar(dial_menu_cursor); // redraw the value back in its normal (non-active) color
	qp_flush(bb_display);
}

// Render the value of "DIAL SETTINGS" item 'item_pos' (1-based) in its sidebar - same role as menu_render_sidebar(),
// simplified since this page never paginates (row is always item_pos - 1)
void dial_menu_render_sidebar(uint8_t item_pos) {
	uint8_t row = item_pos - 1;

	qp_rect(bb_display,
	        MENU_SIDEBAR_TEXT_POSX, MENU_POSY + row*MENU_LINE_HEIGHT,
	        DISPLAY_WIDTH - 1, MENU_POSY + (row+1)*MENU_LINE_HEIGHT,
	        MENU_BACKGROUND, true);

	char value_str[16];
	dial_items[item_pos - 1].get_value(value_str, sizeof(value_str));
	menu_truncate_to_width(value_str, MENU_FONT, MENU_SIDEBAR_MAX_TEXTWIDTH);

	if (value_str[0] != '\0') {
		bool is_active = (ui_get_mode() == UI_MODE_DIAL_EDIT && item_pos == dial_menu_cursor);
		if (is_active) {
			qp_drawtext_recolor(bb_display,
			                    MENU_SIDEBAR_TEXT_POSX,
			                    MENU_POSY + row*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2,
			                    MENU_FONT, value_str,
			                    GLOBAL_THEME_COLOR, MENU_BACKGROUND);
		} else {
			qp_drawtext_recolor(bb_display,
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
	qp_rect(bb_display,
	        MENU_POSX, MENU_POSY,
	        MENU_WIDTH, MENU_POSY + LAYERS_MENU_MAXITEMS * LAYERS_MENU_ROW_HEIGHT,
			MENU_BACKGROUND,
			true);
	for (uint8_t i = 0; i < LAYERS_MENU_MAXITEMS; i++) {
		widget_layer_render_layername(i, LAYERS_MENU_BOX_POSX, MENU_POSY + i*LAYERS_MENU_ROW_HEIGHT);
	}
	// The icon-picker strip (below) stays hidden here - it only appears while
	// actually picking (UI_MODE_LAYERS_PICK, see layers_menu_action()).
}

// Icon picker, shown beneath the last layer box only while in
// UI_MODE_LAYERS_PICK (entered via layers_menu_action()): every icon in the
// shared layer-icon pool (LAYER_ICON_POOL_COUNT, qp_widget_layer.c) laid out
// left-to-right on a grid. layers_menu_scroll_step() shifts it by exactly one
// icon-width per encoder detent (no auto-scrolling). The icon sitting in the
// highlighted middle cell is what layers_menu_submenu_save() commits via
// layer_icon_set_choice() - which is also what widget_layer_render_layername()
// draws inside the box, so the choice feeds straight back into the list (and
// everywhere else that layer's box is shown).
#define LAYERS_MENU_SCROLL_TOP        (MENU_POSY + LAYERS_MENU_MAXITEMS * LAYERS_MENU_ROW_HEIGHT)
#define LAYERS_MENU_SCROLL_HEIGHT     30 // fits between the last box (ends at LAYERS_MENU_SCROLL_TOP) and the chrome's divider line (y=208)
#define LAYERS_MENU_SCROLL_ICON_SIZE  22
#define LAYERS_MENU_SCROLL_ICON_GAP   9
#define LAYERS_MENU_SCROLL_CELL_W     (LAYERS_MENU_SCROLL_ICON_SIZE + LAYERS_MENU_SCROLL_ICON_GAP)
#define LAYERS_MENU_SCROLL_TOTAL_W    (LAYER_ICON_POOL_COUNT * LAYERS_MENU_SCROLL_CELL_W)
#define LAYERS_MENU_SCROLL_VISIBLE    12 // generous upper bound on cells that can be on-screen at once - extras are just skipped
#define LAYERS_MENU_SELECTOR_CELL     4  // the 5th (middle) visible cell - see layers_menu_draw_icon_scroll()
#define LAYERS_MENU_SELECTOR_COLOR    MENU_CURSOR_COLOR

static uint16_t layers_menu_scroll_x = 0; // always an exact multiple of LAYERS_MENU_SCROLL_CELL_W - keeps the grid pixel-aligned so the selector box lines up with an icon exactly

static void layers_menu_draw_icon_scroll(void) {
	uint16_t icon_y      = LAYERS_MENU_SCROLL_TOP + (LAYERS_MENU_SCROLL_HEIGHT - LAYERS_MENU_SCROLL_ICON_SIZE)/2;
	uint16_t first_index = layers_menu_scroll_x / LAYERS_MENU_SCROLL_CELL_W;

	for (uint8_t v = 0; v < LAYERS_MENU_SCROLL_VISIBLE; v++) {
		int32_t x = (int32_t)MENU_POSX + v*LAYERS_MENU_SCROLL_CELL_W +14;
		// Only draw icons that fit fully on-screen: qp_drawimage() takes uint16_t
		// coordinates (no negative x) and doesn't crop images, so a partially
		// visible one at either edge (including the arrows below) is skipped instead.
		if (x + LAYERS_MENU_SCROLL_ICON_SIZE > MENU_WIDTH - 10) continue;

		uint8_t idx = (first_index + v) % LAYER_ICON_POOL_COUNT;
		qp_drawimage(bb_display, (uint16_t)x, icon_y, layer_icon_pool_icon(idx));
	}

	// Outline the middle cell - that's the one that gets saved when
	// layers_menu_submenu_save() runs.
	uint16_t selector_x = MENU_POSX + LAYERS_MENU_SELECTOR_CELL*LAYERS_MENU_SCROLL_CELL_W +14;
	qp_rect(bb_display,
	        selector_x - 3, icon_y - 3,
	        selector_x + LAYERS_MENU_SCROLL_ICON_SIZE + 3, icon_y + LAYERS_MENU_SCROLL_ICON_SIZE + 3,
			LAYERS_MENU_SELECTOR_COLOR, false);
	qp_rect(bb_display,
	        selector_x - 2, icon_y - 2,
	        selector_x + LAYERS_MENU_SCROLL_ICON_SIZE + 2, icon_y + LAYERS_MENU_SCROLL_ICON_SIZE + 2,
			LAYERS_MENU_SELECTOR_COLOR, false);
}

// Icon index currently sitting in the highlighted selector cell.
static uint8_t layers_menu_selected_icon(void) {
	return (layers_menu_scroll_x/LAYERS_MENU_SCROLL_CELL_W + LAYERS_MENU_SELECTOR_CELL) % LAYER_ICON_POOL_COUNT;
}

void layers_menu_action(void) { // Button 2 on a LAYERS CONFIG row: pick its icon (UI_MODE_LAYERS_PICK)
	ui_set_mode(UI_MODE_LAYERS_PICK);

	// Seed the strip so the layer's current icon starts out under the selector
	uint8_t current = layer_icon_get_choice(layers_menu_cursor - 1);
	uint8_t first_index = (current + LAYER_ICON_POOL_COUNT - LAYERS_MENU_SELECTOR_CELL) % LAYER_ICON_POOL_COUNT;
	layers_menu_scroll_x = first_index * LAYERS_MENU_SCROLL_CELL_W;

	qp_rect(bb_display,
	        MENU_POSX, LAYERS_MENU_SCROLL_TOP,
	        MENU_WIDTH, LAYERS_MENU_SCROLL_TOP + LAYERS_MENU_SCROLL_HEIGHT,
			HSV_WHITE,
			true);
	uint16_t arrow_y = LAYERS_MENU_SCROLL_TOP + (LAYERS_MENU_SCROLL_HEIGHT - MENU_CURSOR_ICON_HEIGHT)/2;
	qp_drawimage_recolor(bb_display, MENU_POSX, arrow_y, ico16_arrow_left, GLOBAL_THEME_COLOR, HSV_WHITE);
	qp_drawimage_recolor(bb_display, MENU_WIDTH - MENU_CURSOR_ICON_WIDTH, arrow_y, ico16_arrow_right, GLOBAL_THEME_COLOR, HSV_WHITE);

	layers_menu_draw_icon_scroll();
	qp_flush(bb_display);
}

// Shared by both ways out of the picker: hide the strip, redraw this layer's
// box (reflecting whatever eepdata.layer_icon[] currently holds - a fresh
// choice if just saved, or the original if just cancelled), back to the list.
static void layers_menu_submenu_close(void) {
	uint8_t layer = layers_menu_cursor - 1;
	ui_set_mode(UI_MODE_LAYERS_LIST);

	qp_rect(bb_display,
	        MENU_POSX, LAYERS_MENU_SCROLL_TOP,
	        MENU_WIDTH, LAYERS_MENU_SCROLL_TOP + LAYERS_MENU_SCROLL_HEIGHT,
			MENU_BACKGROUND,
			true);

	widget_layer_render_layername(layer, LAYERS_MENU_BOX_POSX, MENU_POSY + layer*LAYERS_MENU_ROW_HEIGHT);
	qp_flush(bb_display);
}

void layers_menu_submenu_save(void) { // Button 2 (OK) while picking: commit the icon under the selector
	layer_icon_set_choice(layers_menu_cursor - 1, layers_menu_selected_icon());
	layers_menu_submenu_close();
}

void layers_menu_submenu_exit(void) { // Button 1 (Exit) while picking: discard, eepdata.layer_icon[] untouched
	layers_menu_submenu_close();
}

void layers_menu_scroll_step(bool clockwise) { // encoder rotation while picking - one full icon-width per detent
	if (clockwise) {
		layers_menu_scroll_x = (layers_menu_scroll_x + LAYERS_MENU_SCROLL_CELL_W) % LAYERS_MENU_SCROLL_TOTAL_W;
	} else {
		layers_menu_scroll_x = (layers_menu_scroll_x == 0) ? (LAYERS_MENU_SCROLL_TOTAL_W - LAYERS_MENU_SCROLL_CELL_W) : layers_menu_scroll_x - LAYERS_MENU_SCROLL_CELL_W;
	}
	layers_menu_draw_icon_scroll();
	qp_flush(bb_display);
}

void layers_menu_set_cursor(uint8_t cursor_pos) { // cursor_pos is ABSOLUTE (1..LAYERS_MENU_MAXITEMS), single page only
	static uint8_t last_cursor_pos = 0; // 0 = none drawn yet

	uint8_t row = cursor_pos - 1;

	if (last_cursor_pos != 0 && last_cursor_pos != cursor_pos) {
		uint8_t last_row = last_cursor_pos - 1;
		qp_rect(bb_display,
		        MENU_POSX,
		        MENU_POSY + last_row*LAYERS_MENU_ROW_HEIGHT,
		        MENU_POSX + MENU_CURSOR_ICON_WIDTH - 1,
		        MENU_POSY + (last_row+1)*LAYERS_MENU_ROW_HEIGHT,
		        MENU_BACKGROUND,
		        true
			);
	}
	qp_drawimage_recolor(bb_display,
						MENU_POSX,
						MENU_POSY + row*LAYERS_MENU_ROW_HEIGHT + (WIDGET_LAYER_HEIGHT - MENU_CURSOR_ICON_HEIGHT)/2,
						ico16_arrow_right,
						MENU_CURSOR_COLOR,
						MENU_BACKGROUND
					);

	last_cursor_pos = cursor_pos;
}

void layers_menu_open(void) { // Button 2 on MENU_LAYERS_CONFIG
	ui_set_mode(UI_MODE_LAYERS_LIST);
	layers_menu_cursor = MENU_1STLINE_POS;

	menu_draw_chrome("LAYERS CONFIG");
	layers_menu_printlist();
	layers_menu_set_cursor(layers_menu_cursor);
	qp_flush(bb_display);
}

void layers_menu_exit(void) { // Button 1: back to the main "SETTINGS" list
	ui_set_mode(UI_MODE_MENU_LIST);
	main_menu_render();
	qp_flush(bb_display);
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
	qp_rect(bb_display,
	        MENU_SIDEBAR_TEXT_POSX, MENU_POSY + row*MENU_LINE_HEIGHT,
	        DISPLAY_WIDTH - 1, MENU_POSY + (row+1)*MENU_LINE_HEIGHT,
	        MENU_BACKGROUND, true);

	const menu_item_t *item = &menu_items[item_pos - 1];
	if (!item->get_value) return; // e.g. sub-page openers - no in-place value

	char value_str[16];
	item->get_value(value_str, sizeof(value_str));
	menu_truncate_to_width(value_str, MENU_FONT, MENU_SIDEBAR_MAX_TEXTWIDTH);

	if (value_str[0] != '\0') {
		bool is_active = (ui_get_mode() == UI_MODE_MENU_EDIT && item_pos == menu_cursor);
		if (item->value_hue) {
			// The value IS a color (THEME COLOR) - always drawn in it, active or not.
			// menu_process_rotation() re-renders this on every knob tick while editing, so it updates live.
			qp_drawtext_recolor(bb_display,
			                    MENU_SIDEBAR_TEXT_POSX,
			                    MENU_POSY + row*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2,
			                    MENU_FONT, value_str,
			                    item->value_hue(), 255, 255,
			                    MENU_BACKGROUND);
		} else if (is_active) {
			qp_drawtext_recolor(bb_display,
			                    MENU_SIDEBAR_TEXT_POSX,
			                    MENU_POSY + row*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2,
			                    MENU_FONT, value_str,
			                    GLOBAL_THEME_COLOR, MENU_BACKGROUND);
		} else {
			qp_drawtext_recolor(bb_display,
			                    MENU_SIDEBAR_TEXT_POSX,
			                    MENU_POSY + row*MENU_LINE_HEIGHT + (MENU_LINE_HEIGHT - MENU_FONT_HEIGHT)/2,
			                    MENU_FONT, value_str,
			                    HSV_WHITE, MENU_BACKGROUND);
		}
	}
}

void menu_action(void) {
	const menu_item_t *item = &menu_items[menu_cursor - 1];
	if (item->on_rotate) {
		ui_set_mode(UI_MODE_MENU_EDIT);
		uint8_t row = (menu_cursor - 1) % MENU_LINESPERPAGE;
		menu_render_sidebar(menu_cursor, row); // redraw the value in the active (editing) color
	} else if (item->on_select) {
		item->on_select();
	}
}

void action_aboutbuildbox(void) {
	ui_set_mode(UI_MODE_MENU_ABOUT);
	qp_rect(bb_display, 0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, MENU_BACKGROUND, true); // Clear screen
	qp_drawtext(bb_display, 0, MENU_FONT_HEIGHT*1, MENU_FONT, "      BUILDBOX     ");
	qp_drawtext(bb_display, 0, MENU_FONT_HEIGHT*2, MENU_FONT, "A MULTI-FUNCTION MACROPAD");
	// Add QR code & web link here
	qp_flush(bb_display);
}

void action_resettodfu(void) {
	// 1st OK (from the main list): show the confirmation screen. 2nd OK (Button 2
	// in UI_MODE_MENU_DFU_CONFIRM, see ui_button_actions[]): actually reset.
	if (ui_get_mode() != UI_MODE_MENU_DFU_CONFIRM) {
		ui_set_mode(UI_MODE_MENU_DFU_CONFIRM);

		qp_rect(bb_display, 0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, MENU_BACKGROUND, true); // Clear screen
		bb_drawtext_recolor_center(bb_display, DISPLAY_WIDTH/2, TUTORIAL_TITLE_POSY, TUTORIAL_TITLE_FONT,
			"Enter Bootloader Mode",
			HSV_WHITE, MENU_BACKGROUND);
		qp_drawtext_recolor(bb_display, 20, TUTORIAL_TITLE_POSY*3, TUTORIAL_BODY_FONT,
			"- An external drive will appear in",
			HSV_WHITE, MENU_BACKGROUND);
		qp_drawtext_recolor(bb_display, 20, TUTORIAL_TITLE_POSY*4, TUTORIAL_BODY_FONT,
			"                     your computer",
			HSV_WHITE, MENU_BACKGROUND);
		qp_drawtext_recolor(bb_display, 20, TUTORIAL_TITLE_POSY*6, TUTORIAL_BODY_FONT,
			"- Copy firmware file to it",
			HSV_WHITE, MENU_BACKGROUND);

		qp_circle(bb_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
		bb_drawtext_recolor_center(bb_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Cancel", HSV_WHITE, MENU_BACKGROUND);
		qp_circle(bb_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
		bb_drawtext_recolor_center(bb_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "OK", HSV_WHITE, MENU_BACKGROUND);
		qp_flush(bb_display);
		return;
	}

	// rgb_matrix_set_color_all(RGB_BLACK);
	eeprom_custom_save(); // persist this menu session's changes - the reset never leaves the menu through ui_set_mode()
	reset_keyboard();
}

void action_breakout(void) {
	menu_cursor = MENU_1STLINE_POS;
	breakout_open(); // leaves the menu (ui_set_mode() saves eepdata) - Breakout owns the screen now
}

void action_tutorial(void) {
	menu_cursor = MENU_1STLINE_POS;
	tutorial_start(); // leaves the menu (ui_set_mode() saves eepdata) - the tutorial owns the screen now
}

void action_debug(void) {
	ui_set_mode(UI_MODE_MENU_DEBUG);
	char buf[40]; // longest line is "+ theme_color: [ 255, 255, 255 ]" (33 chars + null)
	uint8_t line = 0;
	qp_rect(bb_display, 0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, MENU_BACKGROUND, true); // Clear screen

	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16, "*** DEBUG ***");

	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16, "Display");
	snprintf(buf, sizeof(buf), "+ resolution: %d*%d px", DISPLAY_WIDTH, DISPLAY_HEIGHT);
	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16,
		#if defined(QUANTUM_PAINTER_ST7789_SPI_ENABLE)
		"+ driver: ST7789"
		#else
		"+ driver: ILI9341"
		#endif
	);

	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16, "EEPROM");
	snprintf(buf, sizeof(buf), "+ layer:%d anim:%d",     eepdata.active_layer, eepdata.display_bootanim);
	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ timeout:%d bright:%d", eepdata.display_timeout, eepdata.display_brightness);
	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ rot:%d lly:%d", eepdata.knob_effect, eepdata.lighting_layers);
	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ hue: %d %d %d %d",     eepdata.layer_hue[0], eepdata.layer_hue[1], eepdata.layer_hue[2], eepdata.layer_hue[3]);
	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ sat: %d %d %d %d",     eepdata.layer_sat[0], eepdata.layer_sat[1], eepdata.layer_sat[2], eepdata.layer_sat[3]);
	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ knob:%d ver:%d",       eepdata.knob_func, eepdata.layout_version);
	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);
	snprintf(buf, sizeof(buf), "+ theme_color: [ %d, 255, 255 ]", eepdata.theme_hue);
	qp_drawtext(bb_display, 0, nanoplex16->line_height*line++, nanoplex16, buf);

	qp_flush(bb_display);
}

// void action_factoryreset(void) {
// 	clear_keyboard();   // release all pressed keys if available
// 	eeprom_custom_reset();
// 	eeconfig_disable();
// 	soft_reset_keyboard();
// }

// Knob rotation while any Settings Menu page is open: one call per MENU_STEP_SIZE
// detent (dispatched from knob_step_actions[], sensor/sensors_handler.c). Moves the
// cursor on list pages, and edits the selected value on the in-place edit pages.
// A new menu page only needs a case here.
void menu_process_rotation(bool clockwise) {
	switch (ui_get_mode()) {
		case UI_MODE_MENU_LIST: {
			// Step to the next selectable item (CW => DOWN, CCW => UP), wrapping around
			// both ends and skipping every non-selectable row (divider lines) - works
			// for any item count, page count, and divider position (incl. first/last).
			uint8_t old_cursor = menu_cursor;
			do {
				if (clockwise) {
					menu_cursor = (menu_cursor >= MENU_MAXITEMS) ? 1 : menu_cursor + 1;
				} else {
					menu_cursor = (menu_cursor <= 1) ? MENU_MAXITEMS : menu_cursor - 1;
				}
			} while (!menu_item_is_selectable(menu_cursor) && menu_cursor != old_cursor);

			// Re-print the list whenever the cursor lands on a different page
			if ((old_cursor - 1) / MENU_LINESPERPAGE != (menu_cursor - 1) / MENU_LINESPERPAGE) {
				menu_printlist();
			}
			menu_set_cursor(menu_cursor);
			break;
		}
		case UI_MODE_MENU_EDIT: // knob changes the value live
			menu_items[menu_cursor - 1].on_rotate(clockwise);
			menu_render_sidebar(menu_cursor, (menu_cursor - 1) % MENU_LINESPERPAGE);
			qp_flush(bb_display);
			break;

		case UI_MODE_DIAL_LIST: // only a few items, always on one page - no pagination to handle
			if (clockwise) {
				dial_menu_cursor = (dial_menu_cursor >= DIAL_MENU_MAXITEMS) ? 1 : dial_menu_cursor + 1;
			} else {
				dial_menu_cursor = (dial_menu_cursor <= 1) ? DIAL_MENU_MAXITEMS : dial_menu_cursor - 1;
			}
			dial_menu_set_cursor(dial_menu_cursor);
			break;
		case UI_MODE_DIAL_EDIT:
			dial_items[dial_menu_cursor - 1].on_rotate(clockwise);
			dial_menu_render_sidebar(dial_menu_cursor);
			qp_flush(bb_display);
			break;

		case UI_MODE_LAYERS_LIST: // LAYERS_MENU_MAXITEMS items, always on one page
			if (clockwise) {
				layers_menu_cursor = (layers_menu_cursor >= LAYERS_MENU_MAXITEMS) ? 1 : layers_menu_cursor + 1;
			} else {
				layers_menu_cursor = (layers_menu_cursor <= 1) ? LAYERS_MENU_MAXITEMS : layers_menu_cursor - 1;
			}
			layers_menu_set_cursor(layers_menu_cursor);
			break;
		case UI_MODE_LAYERS_PICK: // shifts the icon strip by exactly one icon's width per step
			layers_menu_scroll_step(clockwise);
			break;

		default: // static screens (About, DFU confirm, debug) ignore the knob
			break;
	}
}

#endif // defined(QUANTUM_PAINTER_ENABLE)