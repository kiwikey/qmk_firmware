#pragma once

#include "quantum.h"
#include "display/defines.h"

#define MENU_BACKGROUND         HSV_BLACK
#define MENU_TITLE_COLOR        HSV_BLACK
#define MENU_TITLE_BG           HSV_WHITE
#define MENU_CURSOR_COLOR       GLOBAL_THEME_COLOR

#define MENU_WIDTH              320 - MENU_POSX
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
#define MENU_SIDEBAR_MAX_TEXTWIDTH (MENU_SIDEBAR_TEXT_POSX - MENU_SIDEBAR_TEXT_PADDING) // long values get truncated to fit this

#define MENU_CURSOR_ICON_WIDTH     8  // ico16_arrow_right is 8x16
#define MENU_CURSOR_ICON_HEIGHT    16

#define MENU_PAGINATION_ARROW_WIDTH   16 // ico16_arrow_up/down are 16x8
#define MENU_PAGINATION_ARROW_HEIGHT  8
#define MENU_PAGINATION_ARROW_POSX    295
#define MENU_PAGINATION_UP_POSY       35
#define MENU_PAGINATION_DOWN_POSY     195

#define MENU_1STLINE_POS    1
#define MENU_MAXITEMS       14 // including divider line
#define MENU_LINESPERPAGE   7

/* GLOBAL VARIATIONS - common use */
	/*** menu_state definition:
		0: not in menu
		1: in main menu
		2: in sub menu
		3: in the "DIAL SETTINGS" sub-page
		4: editing one item on the "DIAL SETTINGS" sub-page
		5: in the "LAYERS CONFIG" sub-page
	*****************/
	extern uint8_t menu_state, menu_cursor;
	extern uint8_t dial_menu_cursor;
	extern uint8_t layers_menu_cursor;
	#define NOT_IN_MENU   0
	#define MAIN_MENU     1
	#define SUB_MENU      2
	#define DIAL_MENU     3
	#define DIAL_SUB_MENU 4
	#define LAYERS_MENU   5
/**********************************/

/* GLOBAL PROCEDURES - common use */
	void menu_init(void);
	void menu_exit(void);
	void menu_submenu_exit(void);
	void menu_printlist(void);
	void menu_set_cursor(uint8_t cursor_pos);
	void menu_render_sidebar(uint8_t item_pos, uint8_t row); // item_pos is 1-based; row is 0-based on its page
	void menu_action(void);
	void eeprom_update_custom(void); // TODO
	bool debug_screen_is_active(void); // see action_debug() (qp_menu.c) - encoder rotation is ignored while true
	bool dfu_confirm_screen_is_active(void); // see action_resettodfu() (qp_menu.c) - encoder rotation is ignored while true

	// "DIAL SETTINGS" sub-page (MENU_DIAL_SETTINGS below) - a small nested list,
	// entered from the main list instead of the usual in-place SUB_MENU editing.
	// Its own cursor/button/encoder handling otherwise mirrors the main list's
	// exactly: dial_menu_action() ~ menu_action(), dial_menu_submenu_exit() ~
	// menu_submenu_exit(), DIAL_SUB_MENU ~ SUB_MENU.
	void dial_menu_open(bool from_shortcut);      // Button 2 on MENU_DIAL_SETTINGS (pass false), or the idle-screen 3s-hold shortcut (pass true)
	void dial_menu_exit(void);                    // Button 1 on the list: back to the main list, or all the way out if from_shortcut was true
	void dial_menu_set_cursor(uint8_t cursor_pos); // cursor_pos is 1..DIAL_MENU_MAXITEMS
	void dial_menu_action(void);                   // Button 2 on a list item: enter DIAL_SUB_MENU to edit it
	void dial_menu_submenu_exit(void);             // Button 1 or 2 while editing: back to the DIAL SETTINGS list
	void dial_menu_render_sidebar(uint8_t item_pos); // item_pos is 1-based; no pagination on this page

	// "LAYERS CONFIG" sub-page (MENU_LAYERS_CONFIG below) - placeholders for now,
	// same nested-list shell as "DIAL SETTINGS" (no editing yet, so no LAYERS_SUB_MENU).
	void layers_menu_open(void);                    // Button 2 on MENU_LAYERS_CONFIG
	void layers_menu_exit(void);                    // Button 1: back to the main list
	void layers_menu_set_cursor(uint8_t cursor_pos); // cursor_pos is 1..LAYERS_MENU_MAXITEMS
/**********************************/

enum menu_label_list_references {
	__DUMP = 0,
	MENU_DISPLAY_BRIGHTNESS,
	MENU_DISPLAYTIMEOUT,
	MENU_SCREENSAVER,
	MENU_DIAL_SETTINGS,
	MENU_THEME_COLOR,

	MENU_RGB_BRIGHTNESS,
	MENU_RGB_MODE,
	MENU_LAYERS_CONFIG,
	MENU_INTROANIM,
	DIVIDER_MENU, // Divider line
	MENU_ABOUT,
	MENU_BREAKOUT,
	// MENU_DEBUG,
	MENU_BOOTTODFU,
	MENU_TUTORIAL
};

static const char * const menu_label_list[MENU_MAXITEMS] = {
	"LCD BRIGHTNESS",
	"SCREEN SAVER",
	"BURN-IN PROTECT",
	"DIAL SETTINGS",
	"THEME COLOR",

	"RGB BRIGHTNESS",
	"RGB MODE",
	"LAYERS CONFIG",
	"BUILDBOX INTRO",
	"   ------",
	"ABOUT BUILDBOX",
	"SECRET GAME",
	// "DEBUG",
	"BOOT TO DFU",
	"QUICK TUTORIAL"
};

static const bool menu_label_list_ischangeable[MENU_MAXITEMS+1] = {
	false, // 0 (never checked, just dump value)
	/* the list below */
	true,  // LCD BRIGHTNESS
	true,  // LCD TIMEOUT
	true,  // BURN-IN PROTECT
	false, // DIAL SETTINGS (opens dial_menu_open() instead of an in-place SUB_MENU)
	true,  // THEME COLOR

	true,  // RGB BRIGHTNESS
	true,  // RGB MODE
	false, // LAYERS CONFIG (opens layers_menu_open() instead of an in-place SUB_MENU)
	true,  // BUILDBOX INTRO
	false, // ------
	false, // ABOUT BUILDBOX
	false, // SECRET GAME
	// false, // DEBUG
	false, // BOOT TO DFU
	false  // QUICK TUTORIAL
};

#define DIAL_MENU_MAXITEMS 3
enum dial_menu_label_list_references { // 1-based, matches dial_menu_label_list[] positions below
	DIAL_MENU_FUNCTION = 1,
	DIAL_MENU_RGB_MODE,
	DIAL_MENU_SENSITIVITY
};
static const char * const dial_menu_label_list[DIAL_MENU_MAXITEMS] = {
	"DIAL FUNCTION",
	"DIAL RGB MODE",
	"DIAL SENSITIVITY"
};

static const bool dial_menu_label_list_ischangeable[DIAL_MENU_MAXITEMS+1] = {
	false, // 0 (never checked, just dump value)
	true,  // DIAL FUNCTION
	true,  // DIAL RGB MODE
	true,  // DIAL SENSITIVITY
};

// "LAYERS CONFIG" sub-page: one line per layer, labeled with its real name
// (layer_names[], display/defines.h) - not wired to any setting yet.
#define LAYERS_MENU_MAXITEMS DYNAMIC_KEYMAP_LAYER_COUNT

// Index 0 is "OFF" (no screensaver at all, regardless of LCD Timeout); indices
// 1..4 map to effects[0..3] in qp_widget_screensaver.c (see SCREENSAVER_OFF_INDEX there).
#define SCREEN_SAVER_MAXITEMS 5
static const char * const screen_saver_effect_list[SCREEN_SAVER_MAXITEMS] = {
	"OFF",
	"Rain 1",
	"Rain 2",
	"Zzz...",
	"Starry"
};

#define DISPLAY_ANIM_QTY  3

// static const char * const anim_list[DISPLAY_ANIM_QTY+1] = {
	// "none",
	// "Cat",
	// "Dog",
	// "Nyan Cat"
// };

void action_displaybrightness(void);
void action_aboutbuildbox(void);
void action_resettodfu(void);
void action_debug(void);
void action_breakout(void);
void action_tutorial(void);

// void action_factoryreset(void);