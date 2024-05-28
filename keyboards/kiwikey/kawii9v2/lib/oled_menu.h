#ifndef OLED_MENU
#define OLED_MENU

#include "quantum.h"

#define MENU_ENABLE_KEYCODE    KC_F19

#define MENU_START_POS  1 // from 0 to 7
#define MENU_NUMOFLINES 5

#define MENU_TOP_LINE   "---------------------"
#define MENU_TITLE_01   " Layout"
#define MENU_TITLE_02   " OLED Timeout"
#define MENU_TITLE_03   " Keyboard info"
#define MENU_TITLE_04   " Factory Reset"
#define MENU_TITLE_05   " Reset to DFU"

#define MENU_ACTION_01  

// const char menu_str_01[] PROGMEM = " Layout";
// const char menu_str_02[] PROGMEM = " OS";
// const char menu_str_03[] PROGMEM = " Factory Reset";
// const char menu_str_04[] PROGMEM = " Keyboard info";
// const char *const string_table[] PROGMEM = {
	// menu_str_01,
	// menu_str_02,
	// menu_str_03,
	// menu_str_04
	// };


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
bool process_record_menu(uint16_t keycode, keyrecord_t *record);
bool rgb_matrix_indicators_kb(void);

void menu_action(void);
void action_factoryreset(void);

#endif /* OLED_MENU */
