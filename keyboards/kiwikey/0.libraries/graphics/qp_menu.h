#ifndef QP_MENU
#define QP_MENU

#include "quantum.h"

#define QP_TIMEOUT_MIN    30	// seconds
#define QP_TIMEOUT_STEP   30
#define QP_TIMEOUT_MAX    180
#define QP_TIMEOUT_NEVER  QP_TIMEOUT_MAX + QP_TIMEOUT_STEP

#define QP_ADJUSTCOLOR_STEP 14 // For changing RGB color of each layer

#define MAINMENU_1STLINE_POS  1 // from 0 to 7
#define MAINMENU_MAXITEMS     9
#define MAINMENU_LINESPERPAGE 5

static const char * const menu_list[MAINMENU_MAXITEMS] = {
	" 1.Active layer",
	" 2.Animation",
	" 3.Screen timeout",
	" 4.Lighting layers",
	" 5.Adjust color",	// TODO
	" 6.FW Version",
	" 7.About ...",
	" 8.Factory Reset",
	" 9.Boot to DFU"
};

static const bool menu_list_ischangable[MAINMENU_MAXITEMS] = {
	true,  // 1
	true,  // 2
	true,  // 3
	true,  // 4
	true,  // 5
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
extern uint8_t current_menu, menu_execute;
#define NOT_IN_MENU 0
#define MAIN_MENU   1
#define SUB_MENU    2

#define QP_ANIM_QTY    3  // QMK logo, Mario, Bongo Cat

static const char * const anim_list[QP_ANIM_QTY+1] = {
	"none",
	"QMK Logo",
	"Mario",
	"Bongo Cat"
};


#endif /* QP_MENU */
