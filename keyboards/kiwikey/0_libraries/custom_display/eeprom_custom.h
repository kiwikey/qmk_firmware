#ifndef EEPROM_CUSTOM
#define EEPROM_CUSTOM
#pragma once
#include "quantum.h"

typedef struct {
	uint8_t active_layer;
	uint8_t display_anim;
	uint8_t display_timeout;
	uint8_t display_brightness;
	uint8_t display_rotation;
	uint8_t lighting_layers; // 0 = OFF; 1 = ON
	uint8_t lighting_flags;  // 0 = UNDERGLOW; 1 = BACKLIGHT; 2 = ALL LEDS
	uint8_t layer_hue[DYNAMIC_KEYMAP_LAYER_COUNT-1];
	uint8_t layer_sat[DYNAMIC_KEYMAP_LAYER_COUNT-1];
	uint8_t knob_func;
	uint8_t checksum;
} EEPROM_CUSTOM_DATA;

extern EEPROM_CUSTOM_DATA eepdata;
extern EEPROM_CUSTOM_DATA eepdata_default;

#endif /* EEPROM_CUSTOM */
