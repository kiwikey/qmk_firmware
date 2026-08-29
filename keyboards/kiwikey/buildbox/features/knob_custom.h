#pragma once

#define KNOB_LED_START         22 // first of the 8 "indicator"-flagged LEDs ringing the knob
#define KNOB_LED_COUNT         8
#define KNOB_LED_FLASH_MS      500 // how long the gradient stays visible after a knob movement
#define KNOB_LED_FALLOFF_ANGLE 96  // ~3 LED-spacings; gives a visible blend on 2 neighbors each side

// eepdata.knob_effect values, cycled in process_encoder_rotate() (sensors_handler.c) via the MENU_KNOB_RGB menu item
#define KNOB_EFFECT_OFF     0 // ring LEDs fully off
#define KNOB_EFFECT_DEFAULT 1 // ring LEDs left untouched, so RGB Matrix's own effect shows through
#define KNOB_EFFECT_LAYER   2 // ring LEDs show the current layer's color, with a gradient on movement

void knob_effect(void);

// eepdata.knob_func values: what encoder rotation does on the main screen (NOT_IN_MENU),
// dispatched in magnetic_encoder_update_kb() (sensors_handler.c). Cycled in
// process_encoder_rotate() (sensors_handler.c) via the MENU_KNOB_FUNC menu item.
#define KNOB_FUNC_HSCROLL 0 // "HS"  - Horizontal Scroll: MS_WHLU / MS_WHLD
#define KNOB_FUNC_VSCROLL 1 // "VS"  - Vertical Scroll:   MS_WHLL / MS_WHLR
#define KNOB_FUNC_VOLUME  2 // "VOL" - Volume control:    KC_VOLU / KC_VOLD
#define KNOB_FUNC_CUSTOM  3 // "CUS" - Custom users' function: no built-in action yet
#define KNOB_FUNC_COUNT   4

// Short text shown centered in the knob widget, indexed by eepdata.knob_func.
static const char * const knob_func_short_text[KNOB_FUNC_COUNT] = {
	"HS",
	"VS",
	"VOL",
	"CUS",
};

// Short text shown centered in the knob widget, indexed by eepdata.knob_func.
static const char * const knob_func_menu_text[KNOB_FUNC_COUNT] = {
	"H-SCR",
	"V-SCR",
	"VOLUME",
	"CUSTOM",
};
