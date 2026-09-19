#pragma once

#include "features/eeprom_custom.h" // eepdata.theme_hue, see GLOBAL_THEME_COLOR below

#define NULL_VALUE 0

#define CW         true
#define CCW        false

// hue is user-adjustable (Settings Menu: MENU_THEME_COLOR) and EEPROM-persisted
// via eepdata.theme_hue; sat/val stay fixed since there's no theme_sat field.
#define GLOBAL_THEME_COLOR  eepdata.theme_hue, 255, 255
#define GLOBAL_BG_COLOR     HSV_BLACK

// eepdata.theme_hue is picked from a fixed list of named presets in
// MENU_THEME_COLOR (knob cycles through this array, see sensors_handler.c)
// instead of a continuous hue wheel - hue values are QMK's own HSV_*
// constants (quantum/color.h) for recognizability.
typedef struct {
	uint8_t     hue;
	const char *name;
} theme_color_preset_t;

#define THEME_COLOR_PRESET_COUNT 10

static const theme_color_preset_t theme_color_presets[THEME_COLOR_PRESET_COUNT] = {
	{   0, "Red"     },
	{  21, "Orange"  },
	{  43, "Yellow"  },
	{  64, "Lime"    },
	{  85, "Green"   },
	{ 128, "Cyan"    },
	{ 149, "Sky Blue"},
	{ 170, "Blue"    },
	{ 191, "Purple"  },
	{ 213, "Magenta" },
};

// Index into theme_color_presets[] matching `hue` exactly, or 0 if it isn't
// one of the presets (e.g. eepdata.theme_hue still at its EEPROM default).
static inline uint8_t theme_color_preset_index(uint8_t hue) {
	for (uint8_t i = 0; i < THEME_COLOR_PRESET_COUNT; i++) {
		if (theme_color_presets[i].hue == hue) return i;
	}
	return 0;
}

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