#ifndef OLED_MENU
#define OLED_MENU

#include "quantum.h"

#define MENU_ENABLE_KEYCODE    KC_F19

#define MENU_1STLINE_POS  1 // from 0 to 7
#define MENU_NUMOFLINES 5

/*** Matrix keys ***
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

static const char * const menu_list[MENU_NUMOFLINES] = {
	" 1.About Kawii9",
	" 2.OLED Timeout",
	" 3.FW Version",
	" 4.Factory Reset",
	" 5.Boot to DFU"
};

static const bool menu_list_ischangable[MENU_NUMOFLINES] = {
	false,
	true,
	false,
	false,
	false
};

void menu_init(void);
void menu_rgbmatrix_init(void);
void menu_set_cursor(uint8_t cursor_pos);
bool process_record_menu(uint16_t keycode, keyrecord_t *record);
bool rgb_matrix_indicators_kb(void);

void menu_quick_view(uint8_t menu_line);
void menu_action(void);
void action_aboutkawii9(void);
void action_factoryreset(void);
void action_resettodfu(void);

#endif /* OLED_MENU */
