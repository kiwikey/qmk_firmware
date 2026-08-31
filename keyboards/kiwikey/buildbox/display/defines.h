#pragma once

#include "features/eeprom_custom.h" // eepdata.theme_hue, see GLOBAL_THEME_COLOR below

#define NULL_VALUE 0

#define CW         true
#define CCW        false

// hue is user-adjustable (Settings Menu: MENU_THEME_COLOR) and EEPROM-persisted
// via eepdata.theme_hue; sat/val stay fixed since there's no theme_sat field.
#define GLOBAL_THEME_COLOR  eepdata.theme_hue, 255, 255
#define GLOBAL_BG_COLOR     HSV_BLACK

/*** Definitions of (almost) all positions, colors, texts,... ***/

// #define UI_COLOR_BACKGROUND     HSV_BLACK
// #define UI_COLOR_SHADOW         0, 0, 100
// #define UI_COLOR_TEXT_H1        HSV_BLACK
// #define UI_COLOR_TEXT_H2        HSV_BLACK
// #define UI_WIDGET_SHADOW        3 // shadow of widgets, in pixel

// #define WIDGET_RGB_COLOR1     HSV_WHITE // Color of the 1st line
// #define WIDGET_RGB_COLOR2     HSV_GREEN // Color of the 2st line

// #define WIDGET_ENCODER_COLOR1     HSV_PINK  // Color of the 1st line
// #define WIDGET_ENCODER_COLOR2     HSV_GOLD // Color of the 2st line
// #define WIDGET_ENCODER_KNOBCOLORIN      HSV_YELLOW
// #define WIDGET_ENCODER_KNOBCOLOROUT     HSV_GOLDENROD

static const char * const layer_names[DYNAMIC_KEYMAP_LAYER_COUNT] = {
	"Default",
	"Media",
	"Mouse",
	"RGB LED"
};

enum custom_keycodes {
	KC_BUTTON_1 = SAFE_RANGE, // keycode 0x7E40
	KC_BUTTON_2
};