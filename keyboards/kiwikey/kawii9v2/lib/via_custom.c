#include "quantum.h"

#include "via_custom.h"
#include "oled_menu.h"

extern uint32_t key_timer;
extern uint8_t  eepdata_active_layer,
                eepdata_oled_anim,
                eepdata_oled_timeout;
extern uint8_t  eepdata_hue_layer[5],
				eepdata_sat_layer[5];
extern bool		eepdata_layer_indicator;

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
	oled_on(); key_timer = timer_read32(); // turn on OLED
	
	dprintf("via_custom_value_command_kb: %d %d %d %d - %d %d - %d %d \n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
	
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
		
		// DEFAULT LAYER & LAYER INDICATOR
        case id_layer_setactive: {
			eepdata_active_layer = *value_data;
			layer_move(eepdata_active_layer);
            break;
        }
        case id_layer_indicator_enable: {
			eepdata_layer_indicator = *value_data;
            break;
        }
        case id_layer_indicator_layer: { // received an array
			// data[0] is value_id
			// data[1] is array index
			// data[2] is value of HUE
			// data[3] is value of SAT
			eepdata_hue_layer[data[1]] = data[2];
			eepdata_sat_layer[data[1]] = data[3];
            break;
        }

		// OLED CONTROL
        case id_oled_animation: {
			eepdata_oled_anim = *value_data;
            break;
        }
		case id_oled_timeout: {
			eepdata_oled_timeout = (*value_data + 1) * OLED_TIMEOUT_STEP;
			break;
		}
		
		// SYSTEM CONTROL
		case id_soft_reset: {
			soft_reset_keyboard();
			break;
		}
		case id_factory_reset: {
			action_factoryreset();
			break;
		}
		case id_reset_to_dfu: {
			action_resettodfu();
			break;
		}
    }
	// dprintf("- via_config_set_value: %d %d \n", data[0], data[1]);
}

void via_config_get_value( uint8_t *data )
{
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch ( *value_id )
    {

		// DEFAULT LAYER & LAYER INDICATOR
        case id_layer_setactive: {
            value_data[0] = eepdata_active_layer;
            value_data[1] = 0xFF;
            break;
        }
        case id_layer_indicator_enable: {
			value_data[0] = eepdata_layer_indicator;
			value_data[1] = 0xFF;
            break;
        }
        case id_layer_indicator_layer: { // going to send an array
			// value_data[0] is array index
			// value_data[1] is value of HUE
			// value_data[2] is value of SAT
			value_data[0] = data[1];
			value_data[1] = eepdata_hue_layer[data[1]];
			value_data[2] = eepdata_sat_layer[data[1]];
			value_data[3] = 0xFF; // not sure if this is needed
            break;
        }
		
		// OLED CONTROL
        case id_oled_animation: {
            value_data[0] = eepdata_oled_anim;
            value_data[1] = 0xFF;
            break;
        }
		case id_oled_timeout: {
            value_data[0] = (eepdata_oled_timeout / OLED_TIMEOUT_STEP) - 1;
            value_data[1] = 0xFF;
			break;
		}
    }
	dprintf("- via_config_get_value: %d %d %d %d \n", data[0], data[1], data[2], data[3]);
}

void via_config_save(void)
{
	eeprom_update_byte((uint8_t*)EEPROM_ACTIVE_LAYER,    eepdata_active_layer);
	eeprom_update_byte((uint8_t*)EEPROM_OLED_ANIM,       eepdata_oled_anim);
	eeprom_update_byte((uint8_t*)EEPROM_OLED_TIMEOUT,    eepdata_oled_timeout);
	eeprom_update_byte((uint8_t*)EEPROM_LAYER_INDICATOR, eepdata_layer_indicator);
	// Saving all layers' HUE & SAT setting (total 10 numbers)
	for (uint8_t i = 0; i <= 4; i++) {
		eeprom_update_byte((uint8_t*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR+i+4), eepdata_hue_layer[i]);
	}
	for (uint8_t i = 0; i <= 4; i++) {
		eeprom_update_byte((uint8_t*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR+i+9), eepdata_sat_layer[i]);
	}
	// dprint("- via_config_save! \n");
}

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {
    if (!rgb_matrix_indicators_advanced_user(led_min, led_max)) {
        return false;
    }
	if (!eepdata_layer_indicator) // If Layer Indicator is off, there's nothing to do here
		return false;
	
	HSV hsv = {0, 255, rgb_matrix_get_val()};

	// switch (get_highest_layer(layer_state)) {
		// case 0:
			// hsv.h = eepdata_hue_layer[0];
			// break;
		// case 1:
			// rgb_matrix_set_color_all(RGB_MAGENTA);
			// break;
		// case 2:
			// rgb_matrix_set_color_all(RGB_RED);
			// break;
		// case 3:
			// rgb_matrix_set_color_all(RGB_TEAL);
			// break;
		// case 4:
			// rgb_matrix_set_color_all(RGB_YELLOW);
			// break;
		// case 5:
			// break;
	// }
	
	uint8_t i = get_highest_layer(layer_state);
	hsv.h = eepdata_hue_layer[i];

    RGB rgb = hsv_to_rgb(hsv);
	rgb_matrix_set_color_all(rgb.r, rgb.g, rgb.b);
    // for (uint8_t i = led_min; i < led_max; i++) {
        // if (HAS_FLAGS(g_led_config.flags[i], 0x01)) { // 0x01 == LED_FLAG_MODIFIER
            // rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        // }
    // }
    return false;
}