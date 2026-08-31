#pragma once

#include "quantum.h"

// VIA custom values, refer to VIA json file for double-checking

enum via_layer_config_value {
	id_layer_setactive   = 0,
    id_rgb_layers_enable = 1,
	// 2 was id_rgb_layers_flags ("Applied to" LED-group dropdown) - removed,
	// it was never actually consumed by rgb_matrix_indicators_advanced_kb().
	// Left as a gap rather than renumbering everything after it.
    id_rgb_layers_hue    = 3
};

enum via_lcd_value {
    id_boot_animation    = 4,
    id_display_timeout   = 5,
    id_theme_color       = 11
};

enum via_system_value {
	id_soft_reset        = 6,
	id_factory_reset     = 7,
	id_reset_to_dfu      = 8
};

enum via_knob_value {
    id_rgb_wheel         = 9,
    id_knob_func         = 10,
    id_knob_sensitivity  = 12
};


void via_config_set_value( uint8_t *data );
void via_config_get_value( uint8_t *data );
void via_config_save(void);
