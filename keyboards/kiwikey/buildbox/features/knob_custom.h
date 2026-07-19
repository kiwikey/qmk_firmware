#pragma once

#define KNOB_LED_START         22 // first of the 8 "indicator"-flagged LEDs ringing the knob
#define KNOB_LED_COUNT         8
#define KNOB_LED_FLASH_MS      500 // how long the gradient stays visible after a knob movement
#define KNOB_LED_FALLOFF_ANGLE 96  // ~3 LED-spacings; gives a visible blend on 2 neighbors each side

void knob_effect(void);
