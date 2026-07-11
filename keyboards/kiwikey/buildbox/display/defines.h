#pragma once

#define NULL_VALUE 0

/*** Definitions of (almost) all positions, colors, texts,... ***/

// #define UI_COLOR_BACKGROUND     HSV_BLACK
#define UI_COLOR_SHADOW         0, 0, 100
// #define UI_COLOR_TEXT_H1        HSV_BLACK
// #define UI_COLOR_TEXT_H2        HSV_BLACK
#define UI_WIDGET_SHADOW        3 // shadow of widgets, in pixel

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

static const hsv_t layer_colors[] = {
    [0] = { HSV_CYAN },
    [1] = { HSV_MAGENTA },
    [2] = { HSV_YELLOW },
    [3] = { HSV_GREEN },
};

enum custom_keycodes {
	KC_NEXT_LAYER = SAFE_RANGE, // keycode 0x7E40
	KC_MY_MENU
};