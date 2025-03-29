#ifndef _DEFINES
#define _DEFINES

/*** Definitions of (almost) all positions, colors, texts,... ***/

#define UI_COLOR_BACKGROUND     HSV_BLACK
#define UI_COLOR_SHADOW         0, 0, 100
#define UI_COLOR_TEXT_H1        HSV_BLACK
#define UI_COLOR_TEXT_H2        HSV_BLACK
#define UI_WIDGET_SHADOW        3 // shadow of widgets, in pixel

#define MENU_COLOR_BACKGROUND   HSV_BLACK
#define MENU_POSX               0
#define MENU_POSY               40
#define MENU_WIDTH              240

#define WIDGET_LAYER_COLOR1     HSV_WHITE // Color of the 1st line

#define WIDGET_MATRIX_BACKGROUND 0,0,40
#define WIDGET_MATRIX_COLOR_OFF  HSV_WHITE
// #define WIDGET_MATRIX_HUE_ON    HSV_RED

#define WIDGET_RGB_COLOR1     HSV_WHITE // Color of the 1st line
#define WIDGET_RGB_COLOR2     HSV_GREEN // Color of the 2st line

// #define WIDGET_ENCODER_COLOR1     HSV_PINK  // Color of the 1st line
// #define WIDGET_ENCODER_COLOR2     HSV_GOLD // Color of the 2st line
#define WIDGET_ENCODER_KNOBCOLORIN      HSV_YELLOW
#define WIDGET_ENCODER_KNOBCOLOROUT     HSV_GOLDENROD

static const char * const layer_name[DYNAMIC_KEYMAP_LAYER_COUNT] = {
	"Default",
	" User1 ",
	" User2 ",
	"Configs"
};

#endif /* _DEFINES */
