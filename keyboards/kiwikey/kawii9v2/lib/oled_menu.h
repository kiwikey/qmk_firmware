#ifndef OLED_MENU
#define OLED_MENU

#include "quantum.h"

#define EEPROM_ACTIVE_LAYER (VIA_EEPROM_CUSTOM_CONFIG_ADDR)
#define EEPROM_OLED_ANIM    (VIA_EEPROM_CUSTOM_CONFIG_ADDR+1)
#define EEPROM_OLED_TIMEOUT (VIA_EEPROM_CUSTOM_CONFIG_ADDR+2)

#define OLED_TIMEOUT_MIN    30	// seconds
#define OLED_TIMEOUT_STEP   30
#define OLED_TIMEOUT_MAX    180
#define OLED_TIMEOUT_NEVER  OLED_TIMEOUT_MAX + OLED_TIMEOUT_STEP

#define MAINMENU_1STLINE_POS  1 // from 0 to 7
#define MAINMENU_MAXITEMS     9
#define MAINMENU_LINESPERPAGE 5

static const char * const menu_list[MAINMENU_MAXITEMS] = {
	" 1.Active Layer",
	" 2.Animation",
	" 3.OLED Timeout",
	" 4.?????",
	" 5.?????",
	" 6.FW Version",
	" 7.About Kawii9",
	" 8.Factory Reset",
	" 9.Boot to DFU"
};

static const bool menu_list_ischangable[MAINMENU_MAXITEMS] = {
	true,  // 1
	true,  // 2
	true,  // 3
	false, // 4
	false, // 5
	false, // 6
	false, // 7
	false, // 8
	false  // 9
};

/*** Matrix of controlling keys ***
00 01 02
10 11 12
20 21 22
*******************/
#define MENU_KEY_UP     01
#define MENU_KEY_DOWN   21
#define MENU_KEY_SELECT 11
#define MENU_KEY_EXIT   20
#define MENU_KEY_LEFT   10
#define MENU_KEY_RIGHT  12

/*** current_menu definition:
0: not in menu
1: in main menu
2: in sub menu
*****************/
extern uint8_t current_menu;
#define NOT_IN_MENU 0
#define MAIN_MENU   1
#define SUB_MENU    2

#define OLED_ANIM_QTY    3  // QMK logo, Mario, Bongo Cat

static const char * const anim_list[OLED_ANIM_QTY+1] = {
	"none",
	"QMK Logo",
	"Mario",
	"Bongo Cat"
};


void menu_init(void);
void menu_printlist(void);
void menu_set_cursor(uint8_t cursor_pos);
bool process_record_menu(uint16_t keycode, keyrecord_t *record);
void eeprom_update_custom(void);

void menu_quick_view(uint8_t menu_line);
void menu_action(void);

void action_activelayer(void);
void action_animation(void);
void action_oledtimeout(void);
void action_aboutkawii9(void);
void action_factoryreset(void);
void action_resettodfu(void);

#endif /* OLED_MENU */
