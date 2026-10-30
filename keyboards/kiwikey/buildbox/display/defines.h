#pragma once

#include "features/eeprom_custom.h" // eepdata.theme_hue, see GLOBAL_THEME_COLOR below


#define CW         true
#define CCW        false

// hue is user-adjustable (Settings Menu: MENU_THEME_COLOR) and EEPROM-persisted
// via eepdata.theme_hue; sat/val stay fixed since there's no theme_sat field.
#define GLOBAL_THEME_COLOR  eepdata.theme_hue, 255, 255
#define GLOBAL_BG_COLOR     HSV_BLACK

// eepdata.theme_hue is picked from a fixed list of named presets in
// MENU_THEME_COLOR (knob cycles through theme_color_presets[], see menu_process_rotation() in qp_menu.c)
// instead of a continuous hue wheel - hue values are QMK's own HSV_*
// constants (quantum/color.h) for recognizability.
typedef struct {
	uint8_t     hue;
	const char *name;
} theme_color_preset_t;

// The "DIAL SETTINGS" hold time and the screensaver idle time are user settings now:
// eepdata.dial_hold_100ms / eepdata.screensaver_idle_min (features/eeprom_custom.h), read through
// dial_settings_hold_ms() / screensaver_idle_ms().

#define THEME_COLOR_PRESET_COUNT 10

extern const char * const layer_names[DYNAMIC_KEYMAP_LAYER_COUNT];
