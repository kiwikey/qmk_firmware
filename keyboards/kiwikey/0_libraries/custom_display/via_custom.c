#include "via_custom.h"
#include "eeprom_custom.h"

#if defined(OLED_ENABLE)
	#include "oled.h"
	#include "oled/oled_ui.h"
	#include "oled/oled_menu.h"
#endif // defined(OLED_ENABLE)

#if defined(QUANTUM_PAINTER_ENABLE)
    #include "qp_graphics.h"
	#include "qp/qp_menu.h"
	// #include "qp/qp_ui.h"
	// extern painter_device_t my_display;
	// extern bool display_rotate_flag;
#endif // defined(QUANTUM_PAINTER_ENABLE)

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
	#if defined(OLED_ENABLE)
	oled_on();
	#endif // defined(OLED_ENABLE)
	
#if defined(CONSOLE_ENABLE)
	dprintf("via_custom_value_command_kb: %d %d %d %d - %d %d - %d %d \n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
#endif // defined(CONSOLE_ENABLE)
	
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if ( *channel_id == id_custom_channel ) { // id_custom_channel = 0
        switch ( *command_id ) {
            case id_custom_set_value: { // id_custom_set_value = 0x07
                via_config_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value: { // id_custom_get_value = 0x08
                via_config_get_value(value_id_and_data);
                break;
            }
            case id_custom_save: { // id_custom_save = 0x09
                via_config_save();
                break;
            }
            default: {
                // Unhandled message.
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }
    // Return the unhandled state
    *command_id = id_unhandled;

    // DO NOT call raw_hid_send(data,length) here, let caller do this
}

void via_config_set_value( uint8_t *data )
{
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch ( *value_id ) {
		
		// DEFAULT LAYER & LIGHTING LAYERS
        case id_layer_setactive: {
			eepdata.active_layer = *value_data;
			layer_move(eepdata.active_layer);
            break;
        }
        case id_lighting_layers_enable: {
			eepdata.lighting_layers = *value_data;
            break;
        }
        case id_lighting_layers_flags: {
			eepdata.lighting_flags = *value_data;
            break;
        }
        case id_lighting_layers_layer: { // received an array
			// data[0] is value_id
			// data[1] is array index
			// data[2] is value of HUE
			// data[3] is value of SAT
			eepdata.layer_hue[data[1]] = data[2];
			eepdata.layer_sat[data[1]] = data[3];
            break;
        }

		// OLED CONTROL
		#if defined(OLED_ENABLE)
        case id_display_animation: {
			eepdata.display_anim = *value_data;
			sub_ui_clear(); // need to refresh SubUI when changing animation using VIA
            break;
        }
		case id_display_timeout: {
			eepdata.display_timeout = (*value_data + 1) * DISPLAY_TIMEOUT_STEP;
			break;
		}
		#endif // defined(OLED_ENABLE)
		
		// SYSTEM CONTROL
		case id_soft_reset: {
			soft_reset_keyboard();
			break;
		}
		case id_factory_reset: {
			// action_factoryreset();
			break;
		}
		case id_reset_to_dfu: {
			// action_resettodfu();
			break;
		}
    }
}

void via_config_get_value( uint8_t *data )
{
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch ( *value_id )
    {

		// DEFAULT LAYER & LIGHTING LAYERS
        case id_layer_setactive: {
            value_data[0] = eepdata.active_layer;
            value_data[1] = 0xFF;
            break;
        }
        case id_lighting_layers_enable: {
			value_data[0] = eepdata.lighting_layers;
			value_data[1] = 0xFF;
            break;
        }
        case id_lighting_layers_flags: {
			value_data[0] = eepdata.lighting_flags;
			value_data[1] = 0xFF;
            break;
        }
        case id_lighting_layers_layer: { // going to send an array
			// value_data[0] is array index
			// value_data[1] is value of HUE
			// value_data[2] is value of SAT
			value_data[0] = data[1];
			value_data[1] = eepdata.layer_hue[data[1]];
			value_data[2] = eepdata.layer_sat[data[1]];
			value_data[3] = 0xFF; // not sure if this is needed
            break;
        }
		
		// OLED CONTROL
        case id_display_animation: {
            value_data[0] = eepdata.display_anim;
            value_data[1] = 0xFF;
            break;
        }
		case id_display_timeout: {
            value_data[0] = (eepdata.display_timeout / DISPLAY_TIMEOUT_STEP) - 1;
            value_data[1] = 0xFF;
			break;
		}
    }
}

void via_config_save(void)
{
	eeprom_update_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
}

#if defined(RGB_MATRIX_ENABLE)
bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) { // Lighting Layers
    if (!rgb_matrix_indicators_advanced_user(led_min, led_max)) {
        return false;
    }

	HSV hsv = {
		eepdata.layer_hue[get_highest_layer(layer_state)], // HUE = HUE setting of current layer
		eepdata.layer_sat[get_highest_layer(layer_state)],
		rgb_matrix_get_val() // VAL = current RGBMatrix's brightness
	};
	if (hsv.s == 0) hsv.v = 0;
    RGB rgb = hsv_to_rgb(hsv);

// #if defined(OLED_ENABLE)
	// if (current_menu == SUB_MENU) {
		// if (menu_cursor == 5) { // In SubMenu 5 (Lighting Layer's Color), show the real-time color
			// for (uint8_t i = led_min; i < led_max; i++) {
				// if (g_led_config.flags[i] & ((eepdata.lighting_flags+1) << 1)) {
					// rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
				// }
			// }
			// return false;
		// }
		// return false;
	// }
// #endif // defined(OLED_ENABLE)

	if (!(eepdata.lighting_layers & 0x0F)) // If Lighting Layers is off, there's nothing to do here
		return false;

    for (uint8_t i = led_min; i < led_max; i++) {
		if (g_led_config.flags[i] & ((eepdata.lighting_flags+1) << 1)) {
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }
    return false;
}
#endif // defined(RGB_MATRIX_ENABLE)
