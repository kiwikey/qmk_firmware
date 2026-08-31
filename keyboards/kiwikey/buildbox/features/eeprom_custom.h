#pragma once

#define EEPROM_SIZE 1024 // TODO: re-check this

#include "quantum.h"
#include "eeprom.h"
#include "nvm_eeprom_via_internal.h"
#include "nvm_eeprom_eeconfig_internal.h"

typedef struct {
	uint8_t active_layer;
	uint8_t display_bootanim;
	uint8_t display_timeout;
	uint8_t display_brightness;
	uint8_t lighting_layers; // 0 = OFF; 1 = ON
	uint8_t layer_hue[DYNAMIC_KEYMAP_LAYER_COUNT];
	uint8_t layer_sat[DYNAMIC_KEYMAP_LAYER_COUNT];
	uint8_t knob_effect;
	uint8_t knob_func;
	uint8_t theme_hue;
	uint8_t knob_sensitivity; // LOW/MEDIUM/HIGH index, see KNOB_SENSITIVITY_* and knob_sensitivity_step[] (sensors_handler.h)
	uint8_t unbox_tutorial;   // 1 = show the first-boot tutorial (display/widgets/tutorial.c), 0 = already shown/dismissed
	uint8_t checksum;
} EEPROM_CUSTOM_DATA;

extern EEPROM_CUSTOM_DATA eepdata;
extern EEPROM_CUSTOM_DATA eepdata_default;
