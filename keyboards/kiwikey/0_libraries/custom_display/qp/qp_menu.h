#ifndef QP_MENU
#define QP_MENU

#include "quantum.h"
#include "defines.h" // TODO

/* GLOBAL VARIATIONS - common use */
	/*** current_menu definition:
		0: not in menu
		1: in main menu
		2: in sub menu
	*****************/
	extern uint8_t current_menu, menu_cursor;
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

#define MAINMENU_1STLINE_POS  1 // OLED: from 0 to 7
#define MAINMENU_MAXITEMS     11
#define MAINMENU_LINESPERPAGE 6

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
	MENU_DEBUG
	// MENU_LIGHTINGLAYERS,
};

static const char * const menu_list[MAINMENU_MAXITEMS] = {
	" 1.Active Layer",
	" 2.Animation",
	" 3.LCD Timeout",
	" 4.LCD Brightness",
	" 5.Rotate Keyboard",
	" 6.Knob Rotation Fn",
	" 7.FW Version",
	" 8.About Kiwi5x5",
	" 9.Factory Reset",
	" 10.Boot to DFU",
	" 11.Debug",
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
	false  // 10
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
void action_aboutkiwi5x5(void);
void action_factoryreset(void);
void action_resettodfu(void);
void action_debug(void);

#endif /* QP_MENU */
