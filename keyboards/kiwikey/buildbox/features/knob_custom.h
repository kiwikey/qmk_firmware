#pragma once

#define KNOB_LED_START         22 // first of the 8 "indicator"-flagged LEDs ringing the knob
#define KNOB_LED_COUNT         8
#define KNOB_LED_FLASH_MS      500 // how long the gradient stays visible after a knob movement
#define KNOB_LED_FALLOFF_ANGLE 96  // ~3 LED-spacings; gives a visible blend on 2 neighbors each side

// eepdata.knob_effect values - "RGB MODE" on the DIAL SETTINGS sub-page, cycled in
// menu_process_rotation() (qp_menu.c); also settable from VIA (id_rgb_wheel)
#define KNOB_EFFECT_OFF     0 // ring LEDs fully off
#define KNOB_EFFECT_DEFAULT 1 // ring LEDs left untouched, so RGB Matrix's own effect shows through
#define KNOB_EFFECT_LAYER   2 // ring LEDs show the current layer's color, with a gradient on movement
#define KNOB_EFFECT_COUNT   3

void knob_effect(void);

// eepdata.knob_func values: what encoder rotation does on the idle screen (UI_MODE_IDLE),
// dispatched in knob_on_rotation() (sensors_handler.c) - "FUNCTION" on the DIAL
// SETTINGS sub-page (qp_menu.c); also settable from VIA (id_knob_func)
#define KNOB_FUNC_HSCROLL 0 // "HS"  - Horizontal Scroll: MS_WHLR / MS_WHLL
#define KNOB_FUNC_VSCROLL 1 // "VS"  - Vertical Scroll:   MS_WHLD / MS_WHLU
#define KNOB_FUNC_VOLUME  2 // "VOL" - Volume control:    KC_VOLU / KC_VOLD
#define KNOB_FUNC_CUSTOM  3 // "CUS" - Custom users' function: no built-in action yet
#define KNOB_FUNC_COUNT   4

