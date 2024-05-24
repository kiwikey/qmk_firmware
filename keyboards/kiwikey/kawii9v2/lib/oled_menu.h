#ifndef OLED_MENU
#define OLED_MENU

#include "quantum.h"

#define MENU_ENABLE_KEYCODE    KC_F20

#define MENU_START_POS  1 // from 0 to (7 - max line)
#define MENU_NUMOFLINES 4

#define MENU_TOP_LINE   "---------------------"
#define MENU_TITLE_01   " Layout"
#define MENU_TITLE_02   " OS"
#define MENU_TITLE_03   " Factory Reset"
#define MENU_TITLE_04   " Keyboard info"

/*** Matrix keys ***
00 01 02
10 11 12
20 21 22
*******************/
#define MENU_KEY_UP     01
#define MENU_KEY_DOWN   21
#define MENU_KEY_OK     11
#define MENU_KEY_EXIT   20

extern bool menu_is_enabled;

void menu_init(void);
void menu_rgbmatrix_init(void);
void menu_set_cursor(uint8_t cursor_pos);
void process_record_menu(uint16_t keycode, keyrecord_t *record);
bool rgb_matrix_indicators_kb(void);

#endif /* OLED_MENU */
