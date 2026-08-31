#pragma once

#include "quantum.h"
#include "display/defines.h"

#define MENU_BACKGROUND         HSV_BLACK
#define MENU_TITLE_COLOR        HSV_BLACK
#define MENU_TITLE_BG           HSV_WHITE
#define MENU_CURSOR_COLOR       GLOBAL_THEME_COLOR

#define MENU_WIDTH              320 - MENU_POSX
#define MENU_LABEL_WIDTH        220
#define MENU_SIDEBAR_POSX       MENU_LABEL_WIDTH

#define MENU_TITLE_POSY         20        // centered to LCD's width, so no POSX needed
#define MENU_FONT               nanoplex32
#define MENU_FONT_HEIGHT        (MENU_FONT->line_height)
#define MENU_LINE_HEIGHT        23  // Height for each menu line
#define MENU_POSX               15
#define MENU_POSY               40

#define MENU_SIDEBAR_TEXT_PADDING  10
#define MENU_SIDEBAR_TEXT_POSX     (MENU_SIDEBAR_POSX + MENU_SIDEBAR_TEXT_PADDING)
#define MENU_SIDEBAR_MAX_TEXTWIDTH (MENU_SIDEBAR_TEXT_POSX - MENU_SIDEBAR_TEXT_PADDING) // long values get truncated to fit this

// MENU_THEME_COLOR's sidebar swatch (see menu_render_sidebar()) - fits inside
// MENU_LINE_HEIGHT and starts at MENU_SIDEBAR_TEXT_POSX, well clear of
// MENU_PAGINATION_ARROW_POSX (295)
#define MENU_COLOR_SWATCH_WIDTH   36
#define MENU_COLOR_SWATCH_HEIGHT  14
#define MENU_COLOR_SWATCH_CORNER  4
// Gap between the swatch and each rotate-to-change arrow (ico16_arrow_left/right,
// 8x16), shown flanking it only while its SUB_MENU is open. Total active-state
// width: 8 + gap + 36 + gap + 8 = 60, starting at MENU_SIDEBAR_TEXT_POSX (230),
// still clear of MENU_PAGINATION_ARROW_POSX (295).
#define MENU_COLOR_ARROW_GAP      4

#define MENU_CURSOR_ICON_WIDTH     8  // ico16_arrow_right is 8x16
#define MENU_CURSOR_ICON_HEIGHT    16

#define MENU_PAGINATION_ARROW_WIDTH   16 // ico16_arrow_up/down are 16x8
#define MENU_PAGINATION_ARROW_HEIGHT  8
#define MENU_PAGINATION_ARROW_POSX    295
#define MENU_PAGINATION_UP_POSY       35
#define MENU_PAGINATION_DOWN_POSY     195

#define MENU_1STLINE_POS    1
#define MENU_MAXITEMS       14
#define MENU_LINESPERPAGE   7

/* GLOBAL VARIATIONS - common use */
	/*** menu_state definition:
		0: not in menu
		1: in main menu
		2: in sub menu
	*****************/
	extern uint8_t menu_state, menu_cursor;
	#define NOT_IN_MENU 0
	#define MAIN_MENU   1
	#define SUB_MENU    2
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
/**********************************/

enum menu_label_list_references {
	__DUMP = 0,
	MENU_DISPLAY_BRIGHTNESS,
	MENU_DISPLAYTIMEOUT,
	MENU_RGB_BRIGHTNESS,
	MENU_RGB_MODE,
	MENU_KNOB_RGB,
	MENU_KNOB_FUNC,
	MENU_KNOB_SENSITIVITY,

	MENU_THEME_COLOR,
	MENU_ANIMATION,
	MENU_ABOUT,
	MENU_BREAKOUT,
	MENU_DEBUG,
	MENU_BOOTTODFU,
	MENU_TUTORIAL
};

static const char * const menu_label_list[MENU_MAXITEMS] = {
	"LCD BRIGHTNESS",
	"LCD TIMEOUT",
	"RGB BRIGHTNESS",
	"RGB MODE",
	"RGB SCROLL WHEEL",
	"KNOB FUNCTION",
	"KNOB SENSITIVITY",

	"THEME COLOR",
	"BUILDBOX INTRO",
	"ABOUT BUILDBOX",
	"SECRET GAME",
	"DEBUG",
	"BOOT TO DFU",
	"QUICK TUTORIAL"
};

static const bool menu_label_list_ischangeable[MENU_MAXITEMS+1] = {
	false, // 0 (never checked, just dump value)
	/* the list below */
	true,
	true,
	true,
	true,
	true,
	true,
	true,

	true,
	true,
	false,
	false, // (Breakout Game - triggers immediately, no sub-menu)
	false,
	false,
	false // (Tutorial - triggers immediately, no sub-menu)
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