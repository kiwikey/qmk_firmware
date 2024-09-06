#ifndef QP_MENU
#define QP_MENU

#include "quantum.h"
#include "defines.h"

#define EEPROM_ACTIVE_LAYER    (VIA_EEPROM_CUSTOM_CONFIG_ADDR)
/*** For reference ***
#define EEPROM_OLED_ANIM       (VIA_EEPROM_CUSTOM_CONFIG_ADDR+1)
#define EEPROM_OLED_TIMEOUT    (VIA_EEPROM_CUSTOM_CONFIG_ADDR+2)
#define EEPROM_OLED_BRIGHTNESS (VIA_EEPROM_CUSTOM_CONFIG_ADDR+3)
#define EEPROM_LIGHTING LAYERS (VIA_EEPROM_CUSTOM_CONFIG_ADDR+4)
#define EEPROM_LAYER_HUE       (VIA_EEPROM_CUSTOM_CONFIG_ADDR+5)  // array of 5 numbers
#define EEPROM_LAYER_SAT       (VIA_EEPROM_CUSTOM_CONFIG_ADDR+10) // array of 5 numbers
******/

#define QP_TIMEOUT_MIN    30	// seconds
#define QP_TIMEOUT_STEP   30
#define QP_TIMEOUT_MAX    180
#define QP_TIMEOUT_NEVER  QP_TIMEOUT_MAX + QP_TIMEOUT_STEP

// #define QP_ADJUSTCOLOR_STEP 14 // For changing RGB color of each layer

static const char * const menu_list[MAINMENU_MAXITEMS] = {
	" 1.Active layer",
	" 2.Animation",
	" 3.LCD timeout",
	" 4.LCD brightness",
	" 5.(TO DO)",
	" 6.(TO DO)",	// TODO
	" 7.FW Version",
	" 8.About Kiwi5x5",
	" 9.Factory Reset",
	" 10.Boot to DFU"
};

static const bool menu_list_ischangable[MAINMENU_MAXITEMS] = {
	true,  // 1
	true,  // 2
	true,  // 3
	true,  // 4
	false, // 5
	false, // 6
	false, // 7
	false, // 8
	false, // 9
	false  // 10
};

/*** Matrix of controlling keys ***
???
*******************/
#define MENU_KEY_UP     12
#define MENU_KEY_DOWN   32
#define MENU_KEY_SELECT 22
#define MENU_KEY_EXIT   40
#define MENU_KEY_LEFT   21
#define MENU_KEY_RIGHT  23

/*** current_menu definition:
0: not in menu
1: in main menu
2: in sub menu
*****************/
extern uint8_t current_menu, menu_execute;
#define NOT_IN_MENU 0
#define MAIN_MENU   1
#define SUB_MENU    2

// static const char * const anim_list[QP_ANIM_QTY+1] = {
	// "none",
	// "QMK Logo",
	// "Mario",
	// "Bongo Cat"
// };

void menu_init(void);
void menu_printlist(void);
void menu_set_cursor(uint8_t cursor_pos);
bool process_record_menu(uint16_t keycode, keyrecord_t *record);
void eeprom_update_custom(void);

void menu_quick_view(uint8_t menu_line);
void menu_action(void);

void action_activelayer(void);
void action_animation(void);
void action_displaytimeout(void);
void action_displaybrightness(void);
void action_lightinglayers(void);
void action_lightingconfig(void);
void action_aboutkiwi5x5(void);
void action_factoryreset(void);
void action_resettodfu(void);

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max);

#endif /* QP_MENU */
