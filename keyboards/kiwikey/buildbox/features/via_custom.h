#pragma once

#include "quantum.h"

// VIA custom values, refer to VIA json file for double-checking

enum via_layer_config_value {
	id_layer_setactive   = 0,
    id_rgb_layers_enable = 1,
	id_rgb_layers_flags  = 2,
    id_rgb_layers_hue    = 3
};

enum via_lcd_value {
    id_boot_animation    = 4,
    id_display_timeout   = 5
};

enum via_system_value {
	id_soft_reset        = 6,
	id_factory_reset     = 7,
	id_reset_to_dfu      = 8
};

void via_config_set_value( uint8_t *data );
void via_config_get_value( uint8_t *data );
void via_config_save(void);
