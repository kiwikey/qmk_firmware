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
