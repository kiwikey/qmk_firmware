#ifndef VIA_CUSTOM
#define VIA_CUSTOM

#include "quantum.h"

enum via_lighting_layers_value {
	id_layer_setactive        = 0,
    id_lighting_layers_enable = 1,
	id_lighting_layers_flags  = 2,
    id_lighting_layers_layer  = 3 // array
};

enum via_oled_value {
    id_display_animation = 8,
    id_display_timeout   = 9
};

enum via_system_value {
	id_soft_reset     = 10,
	id_factory_reset  = 11,
	id_reset_to_dfu   = 12
};

void via_config_set_value( uint8_t *data );
void via_config_get_value( uint8_t *data );
void via_config_save(void);

#endif /* VIA_CUSTOM */
