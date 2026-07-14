#pragma once

#include "quantum.h"
#include "display/defines.h"

#define MENU_COLOR_BACKGROUND   HSV_BLACK
#define MENU_CURSOR_OUTLINE     HSV_WHITE
#define MENU_FONT               roboto20
#define MENU_FONT_HEIGHT        (MENU_FONT->line_height)
#define MENU_LINE_HEIGHT        30
#define MENU_POSX               0
#define MENU_POSY               40
#define MENU_WIDTH              320

#define MAINMENU_1STLINE_POS  1
#define MAINMENU_MAXITEMS     12
#define MAINMENU_LINESPERPAGE 6

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
	void menu_printlist(void);
	void menu_set_cursor(uint8_t cursor_pos);
	void menu_quick_view(void);
	void menu_action(void);
	void eeprom_update_custom(void); // TODO
/**********************************/

enum menu_list_references {
	__DUMP = 0,
    MENU_ACTIVATELAYER,
    MENU_ANIMATION,
    MENU_DISPLAYTIMEOUT,
	MENU_DISPLAYBRIGHTNESS,
	MENU_DISPLAYROTATION,
	MENU_KNOBFUNCTION,
    MENU_FWVERSION, // 7
	MENU_ABOUT,
	MENU_FACTORYRESET,
	MENU_BOOTTODFU,
	MENU_DEBUG,
	MENU_BREAKOUT
	// MENU_LIGHTINGLAYERS,
};

static const char * const menu_list[MAINMENU_MAXITEMS] = {
	" Active Layer",
	" Animation",
	" LCD Timeout",
	" LCD Brightness",
	" Rotate Keyboard",
	" Knob Rotation Fn",
	" FW Version",
	" About BuildBox",
	" Factory Reset",
	" Boot to DFU",
	" Debug",
	" Breakout Game",
};

static const bool menu_list_ischangeable[MAINMENU_MAXITEMS+1] = {
	false, // 0 (never checked, just dump value)
	/* the list below */
	true,  // 1
	false, // 2
	true,  // 3
	true,  // 4
	true,  // 5
	true,  // 6
	false, // 7
	false, // 8
	false, // 9
	false, // 10
	false, // 11 (Debug)
	false  // 12 (Breakout Game - triggers immediately, no sub-menu)
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
void action_displayrotation(void);
void action_lightingconfig(void);
void action_aboutbuildbox(void);
void action_factoryreset(void);
void action_resettodfu(void);
void action_debug(void);
void action_breakout(void);
