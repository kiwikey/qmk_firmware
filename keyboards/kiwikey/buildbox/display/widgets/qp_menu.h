#pragma once

#include "quantum.h"
#include "display/defines.h"

#define MENU_BACKGROUND         HSV_BLACK
#define MENU_TITLE_COLOR        HSV_BLACK
#define MENU_TITLE_BG           HSV_WHITE
#define MENU_CURSOR_COLOR       GLOBAL_THEME_COLOR

#define MENU_WIDTH              (DISPLAY_WIDTH - MENU_POSX)
#define MENU_LABEL_WIDTH        210
#define MENU_SIDEBAR_POSX       MENU_LABEL_WIDTH

#define MENU_TITLE_POSY         20        // centered to LCD's width, so no POSX needed
#define MENU_FONT               nanoplex32
#define MENU_FONT_HEIGHT        (MENU_FONT->line_height)
#define MENU_LINE_HEIGHT        23  // Height for each menu line
#define MENU_POSX               10
#define MENU_POSY               40

#define MENU_SIDEBAR_TEXT_PADDING  10
#define MENU_SIDEBAR_TEXT_POSX     (MENU_SIDEBAR_POSX + MENU_SIDEBAR_TEXT_PADDING)
#define MENU_SIDEBAR_MAX_TEXTWIDTH (DISPLAY_WIDTH - MENU_SIDEBAR_TEXT_POSX) // long values get truncated to fit between the sidebar's x and the screen's right edge

#define MENU_CURSOR_ICON_WIDTH     8  // ico16_arrow_right is 8x16
#define MENU_CURSOR_ICON_HEIGHT    16

#define MENU_PAGINATION_ARROW_WIDTH   16 // ico16_arrow_up/down are 16x8
#define MENU_PAGINATION_ARROW_HEIGHT  8
#define MENU_PAGINATION_ARROW_POSX    295
#define MENU_PAGINATION_UP_POSY       35
#define MENU_PAGINATION_DOWN_POSY     195

#define MENU_1STLINE_POS    1
#define MENU_LINESPERPAGE   7
// The item count isn't defined here: it's the size of menu_items[] / dial_items[]
// (qp_menu.c) - add, remove or reorder a row there and everything follows.

/* GLOBAL VARIATIONS - common use */
	// Which page is showing (list / editing a value / sub-page / ...) is the UI
	// mode - ui_get_mode(), display/ui_mode.h. These are each page's cursors.
	extern uint8_t menu_cursor;        // 1-based position in menu_items[]
	extern uint8_t dial_menu_cursor;   // 1-based position in dial_items[]
	extern uint8_t layers_menu_cursor; // 1-based layer number + 1
/**********************************/

/* GLOBAL PROCEDURES - common use */
	void menu_init(void);
	void menu_exit(void);
	void menu_submenu_exit(void);
	void menu_printlist(void);
	void menu_set_cursor(uint8_t cursor_pos);
	void menu_render_sidebar(uint8_t item_pos, uint8_t row); // item_pos is 1-based; row is 0-based on its page
	void menu_action(void);
	void menu_process_rotation(bool clockwise); // knob rotation (one MENU_STEP_SIZE detent) on any menu page - see qp_menu.c

	// "DIAL SETTINGS" sub-page - a small nested list, entered from the main list
	// instead of the usual in-place editing. Its own cursor/button/encoder handling
	// otherwise mirrors the main list's exactly: dial_menu_action() ~ menu_action(),
	// dial_menu_submenu_exit() ~ menu_submenu_exit(), UI_MODE_DIAL_EDIT ~ UI_MODE_MENU_EDIT.
	void dial_menu_open(bool from_shortcut);      // Button 2 on DIAL SETTINGS (pass false), or the idle-screen 3s-hold shortcut (pass true)
	void dial_menu_exit(void);                    // Button 1 on the list: back to the main list, or all the way out if from_shortcut was true
	void dial_menu_set_cursor(uint8_t cursor_pos); // cursor_pos is 1-based
	void dial_menu_action(void);                   // Button 2 on a list item: edit it (UI_MODE_DIAL_EDIT)
	void dial_menu_submenu_exit(void);             // Button 1 or 2 while editing: back to the DIAL SETTINGS list
	void dial_menu_render_sidebar(uint8_t item_pos); // item_pos is 1-based; no pagination on this page

	// "LAYERS CONFIG" sub-page - same nested-list shell as "DIAL SETTINGS":
	// layers_menu_action() ~ dial_menu_action(), UI_MODE_LAYERS_PICK ~ UI_MODE_DIAL_EDIT.
	// Instead of editing a value though, UI_MODE_LAYERS_PICK is an icon picker - the
	// scrolling strip beneath the list (qp_menu.c), stepped one icon-width per
	// encoder detent via layers_menu_scroll_step(). Unlike the in-place edit pages
	// (which apply changes live as you rotate, so Button 1/2 both just exit), the
	// picker only touches eepdata.layer_icon[] on Button 2 - Button 1 discards
	// whatever was scrolled to and leaves it unchanged.
	void layers_menu_open(void);                    // Button 2 on LAYERS CONFIG
	void layers_menu_exit(void);                    // Button 1: back to the main list
	void layers_menu_set_cursor(uint8_t cursor_pos); // cursor_pos is 1..LAYERS_MENU_MAXITEMS
	void layers_menu_action(void);                  // Button 2 on a list item: pick its icon (UI_MODE_LAYERS_PICK)
	void layers_menu_submenu_save(void);            // Button 2 (OK) while picking: commit the icon under the selector, back to the list
	void layers_menu_submenu_exit(void);            // Button 1 (Exit) while picking: discard, back to the list unchanged
	void layers_menu_scroll_step(bool clockwise);   // encoder rotation while picking - shifts the strip by exactly one icon's width
/**********************************/

// "LAYERS CONFIG" sub-page: one line per layer, labeled with its real name
// (layer_names[], qp_widget_layer.c).
#define LAYERS_MENU_MAXITEMS DYNAMIC_KEYMAP_LAYER_COUNT

// Index 0 is "OFF" (no screensaver at all, regardless of LCD Timeout); indices
// 1..4 map to effects[0..3] in qp_widget_screensaver.c (see SCREENSAVER_OFF_INDEX there).
#define SCREEN_SAVER_MAXITEMS 5

// Names of the RGB Matrix effects, indexed by rgb_matrix_get_mode(). QMK numbers the effects enabled in
// keyboard.json ("rgb_matrix" -> "animations") 1, 2, 3... in its own fixed order (quantum/rgb_matrix/animations/
// rgb_matrix_effects.inc), NOT in keyboard.json's order; 0 is RGB_MATRIX_NONE. Keep this table in step with
// keyboard.json: enabling/disabling an effect there shifts every number after it (the _Static_assert catches
// a wrong count, not a wrong order). Full names: the menu's value column is only MENU_SIDEBAR_MAX_TEXTWIDTH wide,
// so a longer name scrolls like a marquee while its row is selected (menu_marquee_task(), qp_menu.c).
// Longest must stay under MENU_VALUE_BUFFER_SIZE (qp_menu.c).
static const char * const rgb_effect_names[] = {
	"00. OFF",                    // RGB_MATRIX_NONE
	"01. SOLID COLOR",            // solid_color
	"02. GRADIENT UP DOWN",       // gradient_up_down
	"03. CYCLE LEFT RIGHT",       // cycle_left_right
	"04. CYCLE UP DOWN",          // cycle_up_down
	"05. RAINBOW MOVING CHEVRON", // rainbow_moving_chevron
	"06. CYCLE OUT IN",           // cycle_out_in
	"07. FLOWER BLOOMING",        // flower_blooming
	"08. RAINDROPS",              // raindrops
	"09. JELLYBEAN RAINDROPS",    // jellybean_raindrops
	"10. PIXEL FRACTAL",          // pixel_fractal
	"11. TYPING HEATMAP",         // typing_heatmap
	"12. DIGITAL RAIN",           // digital_rain
	"13. SOLID REACTIVE",         // solid_reactive
	"14. SOLID REACTIVE CROSS",   // solid_reactive_cross
	"15. STARLIGHT DUAL HUE",     // starlight_dual_hue
	"16. RIVERFLOW",              // riverflow
};
_Static_assert(sizeof(rgb_effect_names) / sizeof(rgb_effect_names[0]) == RGB_MATRIX_EFFECT_MAX, "rgb_effect_names[] (qp_menu.h) must have one entry per enabled RGB Matrix effect + the NONE entry - keep it in step with keyboard.json");

// Marquee for a value too wide for the menu's value column (items with .scroll_value in qp_menu.c's
// menu_items[]): while its row is selected, the text waits MENU_MARQUEE_START_MS, then moves left one
// character every MENU_MARQUEE_STEP_MS, with MENU_MARQUEE_GAP spaces between the end and the restart.
#define MENU_MARQUEE_START_MS  1000
#define MENU_MARQUEE_STEP_MS   150
#define MENU_MARQUEE_GAP       2
void menu_marquee_task(void); // call from housekeeping_task_display(); does nothing outside the main SETTINGS list

void action_aboutbuildbox(void);
void action_resettodfu(void);
void action_debug(void);
void action_breakout(void);
void action_tutorial(void);

// void action_factoryreset(void);