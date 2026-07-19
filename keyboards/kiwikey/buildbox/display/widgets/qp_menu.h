#pragma once

#include "quantum.h"
#include "display/defines.h"

#define MENU_BACKGROUND         HSV_BLACK
#define MENU_TITLE_COLOR        HSV_CYAN
#define MENU_CURSOR_COLOR       HSV_WHITE
#define MENU_FONT               roboto20
#define MENU_FONT_HEIGHT        (MENU_FONT->line_height)
#define MENU_LINE_HEIGHT        30  // Height for each menu line
#define MENU_POSX               0
#define MENU_POSY               40

#define MENU_WIDTH                320 // Full width
#define MENU_LABEL_WIDTH          208 // 65% of MENU_WIDTH
#define MENU_SIDEBAR_WIDTH        (MENU_WIDTH - MENU_LABEL_WIDTH) // 35% of MENU_WIDTH
#define MENU_SIDEBAR_POSX         MENU_LABEL_WIDTH

#define MENU_SIDEBAR_ARROW_WIDTH   8
#define MENU_SIDEBAR_ARROW_HEIGHT  16
#define MENU_SIDEBAR_ARROW_LEFT_X  MENU_SIDEBAR_POSX
#define MENU_SIDEBAR_ARROW_RIGHT_X (MENU_WIDTH - MENU_SIDEBAR_ARROW_WIDTH)

#define MENU_SIDEBAR_TEXT_PADDING  4
#define MENU_SIDEBAR_TEXT_POSX     (MENU_SIDEBAR_POSX + MENU_SIDEBAR_ARROW_WIDTH + MENU_SIDEBAR_TEXT_PADDING)
#define MENU_SIDEBAR_MAX_TEXTWIDTH (MENU_SIDEBAR_TEXT_POSX - MENU_SIDEBAR_TEXT_PADDING) // long values get truncated to fit this

#define MENU_1STLINE_POS    1
#define MENU_MAXITEMS       11
#define MENU_LINESPERPAGE   6

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
/**********************************/

enum menu_label_list_references {
	__DUMP = 0,
    MENU_ACTIVATELAYER,
    MENU_ANIMATION,
    MENU_DISPLAYTIMEOUT,
	MENU_DISPLAYBRIGHTNESS,
	MENU_KNOBFUNCTION,
    MENU_FWVERSION, // 6
	MENU_ABOUT,
	MENU_BREAKOUT,
	MENU_FACTORYRESET,
	MENU_BOOTTODFU,
	MENU_DEBUG
	// MENU_LIGHTINGLAYERS,
};

static const char * const menu_label_list[MENU_MAXITEMS] = {
	" Active Layer",
	" Animation",
	" LCD Timeout",
	" LCD Brightness",
	" Knob Rotation Fn",
	" FW Version",
	" About BuildBox",
	" Breakout Game",
	" Factory Reset",
	" Boot to DFU",
	" Debug",
};

static const bool menu_label_list_ischangeable[MENU_MAXITEMS+1] = {
	false, // 0 (never checked, just dump value)
	/* the list below */
	true,  // 1
	true,  // 2
	true,  // 3
	true,  // 4
	true,  // 5
	false, // 6
	true,  // 7
	false, // 8 (Breakout Game - triggers immediately, no sub-menu)
	false, // 9
	false, // 10
	true   // 11 (Debug)
};

#define DISPLAY_ANIM_QTY  3

// static const char * const anim_list[DISPLAY_ANIM_QTY+1] = {
	// "none",
	// "Cat",
	// "Dog",
	// "Nyan Cat"
// };

void action_activelayer(void);
void action_animation(void);
void action_displaytimeout(void);
void action_displaybrightness(void);
void action_lightingconfig(void);
void action_aboutbuildbox(void);
void action_factoryreset(void);
void action_resettodfu(void);
void action_debug(void);
void action_breakout(void);
