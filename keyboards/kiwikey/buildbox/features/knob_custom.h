#pragma once

#define KNOB_LED_START         22 // first of the 8 "indicator"-flagged LEDs ringing the knob
#define KNOB_LED_COUNT         8
#define KNOB_LED_FLASH_MS      500 // how long the gradient stays visible after a knob movement
#define KNOB_LED_FALLOFF_ANGLE 96  // ~3 LED-spacings; gives a visible blend on 2 neighbors each side

// eepdata.knob_effect values, cycled in process_encoder_rotate() (sensors_handler.c)
// - not currently menu-editable; was MENU_KNOB_RGB, pending the "DIAL SETTINGS" sub-page (qp_menu.c)
#define KNOB_EFFECT_OFF     0 // ring LEDs fully off
#define KNOB_EFFECT_DEFAULT 1 // ring LEDs left untouched, so RGB Matrix's own effect shows through
#define KNOB_EFFECT_LAYER   2 // ring LEDs show the current layer's color, with a gradient on movement
#define KNOB_EFFECT_COUNT   3

void knob_effect(void);

// Sidebar text for "DIAL RGB MODE" (qp_menu.c's DIAL SETTINGS sub-page), indexed by eepdata.knob_effect.
static const char * const knob_effect_short_text[KNOB_EFFECT_COUNT] = {
	"OFF",
	"RGB EFF",
	"LAYER",
};

// eepdata.knob_func values: what encoder rotation does on the main screen (NOT_IN_MENU),
// dispatched in magnetic_encoder_update_kb() (sensors_handler.c) - not currently
// menu-editable; was MENU_KNOB_FUNC, pending the "DIAL SETTINGS" sub-page (qp_menu.c)
#define KNOB_FUNC_HSCROLL 0 // "HS"  - Horizontal Scroll: MS_WHLU / MS_WHLD
#define KNOB_FUNC_VSCROLL 1 // "VS"  - Vertical Scroll:   MS_WHLL / MS_WHLR
#define KNOB_FUNC_VOLUME  2 // "VOL" - Volume control:    KC_VOLU / KC_VOLD
#define KNOB_FUNC_CUSTOM  3 // "CUS" - Custom users' function: no built-in action yet
#define KNOB_FUNC_COUNT   4

// Sidebar text for "DIAL FUNCTION" (qp_menu.c's DIAL SETTINGS sub-page), indexed by eepdata.knob_func.
static const char * const knob_func_long_text[KNOB_FUNC_COUNT] = {
	"H-SCROLL",
	"V-SCROLL",
	"VOLUME",
	"CUSTOM"
};

// Short text shown centered in the knob widget (widget_knob_show_func(), qp_widget_knob.c), indexed by eepdata.knob_func.
static const char * const knob_func_short_text[KNOB_FUNC_COUNT] = {
	"HS",
	"VS",
	"VOL",
	"CUS"
};
