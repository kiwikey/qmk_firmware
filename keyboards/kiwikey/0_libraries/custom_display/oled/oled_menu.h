#ifndef OLED_MENU
#define OLED_MENU

#include "quantum.h"

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

#define DISPLAY_TIMEOUT_MIN    30	// seconds
#define DISPLAY_TIMEOUT_STEP   30
#define DISPLAY_TIMEOUT_MAX    180
#define DISPLAY_TIMEOUT_NEVER  DISPLAY_TIMEOUT_MAX + DISPLAY_TIMEOUT_STEP

#define MAINMENU_1STLINE_POS  1 // OLED: from 0 to 7
#define MAINMENU_MAXITEMS     9
#define MAINMENU_LINESPERPAGE 5

void action_activelayer(void);
void action_animation(void);
void action_oledtimeout(void);
void action_lightinglayers(void);
void action_knobfunction(void);
void action_aboutkawii9(void);
void action_factoryreset(void);
void action_resettodfu(void);

enum menu_list_references {
	__DUMP = 0,
    MENU_ACTIVATELAYER,
    MENU_ANIMATION,
    MENU_OLEDTIMEOUT,
    MENU_LIGHTINGLAYERS,
    MENU_KNOBFUNCTION,
    MENU_FWVERSION,
	MENU_ABOUTKAWII9,
	MENU_FACTORYRESET,
	MENU_BOOTTODFU
};

static const char * const menu_list[MAINMENU_MAXITEMS] = {
	" 1.Active layer",
	" 2.Animation",
	" 3.OLED timeout",
	" 4.Lighting layers",
	" 5.Knob function",	// TODO
	" 6.FW Version",
	" 7.About Kawii9",
	" 8.Factory Reset",
	" 9.Boot to DFU"
};

static const bool menu_list_ischangeable[MAINMENU_MAXITEMS+1] = {
	false, // 0 (never checked, just dump value)
	/* the list below */
	true,  // 1
	true,  // 2
	true,  // 3
	true,  // 4
	true,  // 5
	false, // 6
	true,  // 7
	false, // 8
	false  // 9
};

#define display_anim_QTY    3  // QMK logo, Mario, Bongo Cat

static const char * const anim_list[display_anim_QTY+1] = {
	"none",
	"QMK Logo",
	"Mario",
	"Bongo Cat"
};



#endif /* OLED_MENU */
