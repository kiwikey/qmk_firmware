// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#ifndef KAWII9V2_H
#define KAWII9V2_H

#include "quantum.h"

typedef struct {
    uint8_t active_layer;
    uint8_t oled_anim;
    uint8_t oled_timeout;
    uint8_t lighting_layers; // 0 = OFF; 1 = ON
	uint8_t lighting_flags;  // 0 = UNDERGLOW; 1 = BACKLIGHT; 2 = ALL LEDS
    uint8_t layer_hue[5];
	uint8_t layer_sat[5];
} EEPROM_CUSTOM_DATA;

extern EEPROM_CUSTOM_DATA eepdata;

enum keyboard_keycodes {
    CUSTOM_KC_MENU = QK_KB_0,
    // CUSTOM_2,
    // CUSTOM_3,
};

#endif /* KAWII9V2_H */
