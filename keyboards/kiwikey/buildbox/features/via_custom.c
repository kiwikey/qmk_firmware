#include "via_custom.h"
#include "eeprom_custom.h"
#include "display/qp_graphics.h"

extern uint16_t flag_display_keycode_changed;
extern uint16_t flag_widget_layer_changed;

#if defined(QUANTUM_PAINTER_ENABLE)
    #include "display/qp_graphics.h"
	#include "display/widgets/qp_menu.h"
	// #include "qp/qp_ui.h"
	// extern painter_device_t my_display;
	// extern bool display_rotate_flag;
#endif // defined(QUANTUM_PAINTER_ENABLE)

/* via_custom_value_command_kb only handles 'id_custom_channel'
    Only:
        id_custom_set_value                     = 0x07,
        id_custom_get_value                     = 0x08,
        id_custom_save                          = 0x09,
*/
void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
	// TODO: Turn on LCD here

#if defined(CONSOLE_ENABLE)
	// printf("via_custom_value_command_kb: %d %d %d %d - %d %d - %d %d \n",
    //     data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
#endif // defined(CONSOLE_ENABLE)
	
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
            case id_custom_save: {      // id_custom_save = 0x09
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

/* via_command_kb is where all packets run through, including built-in QMK features and maker's custom features
    We read the packets here
    Return false -> let the via.c handle processing the packet
    Return true  -> via.c will skip everything afterward
*/
bool via_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id   = &(data[0]);
    uint8_t *command_data = &(data[1]); // aka via_channel_id

/* DEBUG: printout full rawHID packet */
    // NOTE: a packet of setting keycode in VIA: [0x05 - layer - row - col - ??? - keycode]
    //       that ??? is unsure, for setting basic keycodes it is 0x00, for macro it is 0x77, for RGB it is 0x78
    //       dig more in 'via.c' line 405
    // printf("receive: command_id = %2d  command_data = [ ", *command_id);
    // for (uint8_t i = 0; i < 5; i++) {
    //     printf("%4d ", command_data[i]);
    // }
    // printf(" ] \n");
/* End of DEBUG */

    // When received 'id_dynamic_keymap_set_keycode' (0x05) : keymap is changed (on VIA app)
    //   -> turn on flag_display_keycode_changed so the LCD refresh the 'Widget Matrix'
    if (*command_id == id_dynamic_keymap_set_keycode) {
        flag_display_keycode_changed = ( 0x1000 | (command_data[0]<<8) | (command_data[1]<<4) | command_data[2]);
    }

	return false;
}

void via_config_set_value( uint8_t *data ) {
    // print("via_config_set_value \n");
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch ( *value_id ) {
		// LAYER-RELATED CONFIGS
        case id_layer_setactive: {
			eepdata.active_layer = *value_data;
			layer_move(eepdata.active_layer); // This triggers layer_state_set_kb()
            break;
        }
        case id_rgb_layers_enable: {
			eepdata.lighting_layers = *value_data;
            break;
        }
        case id_rgb_layers_flags: {
			eepdata.lighting_flags = *value_data;
            break;
        }
        // A packet of "RGB Layer color changing" from VIA:
        //   - First 2 bytes above are already handled in 'via_custom_value_command_kb'
        //      [ ] = id_custom_set_value = 0x07 (command_id)
        //      [ ] = id_custom_channel   = 0x00 (channel_id)
        //   - Then this data[] in 'via_config_set_value'
        // data [0] = id_rgb_layers_hue (0x03)
        //      [1] = index of layer (layer0 = 0, layer1 = 1,...)
        //      [2] = value of HUE
        //      [3] = value of SAT
        // (without this you can never understand my code lol, appreciate this, folk!)
        case id_rgb_layers_hue: {
			eepdata.layer_hue[data[1]] = data[2];
			eepdata.layer_sat[data[1]] = data[3];
            flag_widget_layer_changed = data[1] + 1; // flag need to +1, see 'flag_widget_layer_changed' definition in qp_graphics.c
            break;
        }

		// LCD CONFIGS
        case id_boot_animation: {
			eepdata.display_bootanim = *value_data;
            break;
        }
		case id_display_timeout: {
			eepdata.display_timeout = (*value_data + 1) * DISPLAY_TIMEOUT_STEP;
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
}

void via_config_get_value( uint8_t *data ) {
    // print("via_config_get_value \n");
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch ( *value_id ) {
		// DEFAULT LAYER & LIGHTING LAYERS
        case id_layer_setactive: {
            value_data[0] = eepdata.active_layer;
            break;
        }
        case id_rgb_layers_enable: {
			value_data[0] = eepdata.lighting_layers;
            break;
        }
        case id_rgb_layers_flags: {
			value_data[0] = eepdata.lighting_flags;
            break;
        }
        case id_rgb_layers_hue: { // going to send an array
			// value_data[0] is array index
			// value_data[1] is value of HUE
			// value_data[2] is value of SAT
			value_data[0] = data[1];
			value_data[1] = eepdata.layer_hue[data[1]];
			value_data[2] = eepdata.layer_sat[data[1]];
            break;
        }
		
		// OLED CONTROL
        case id_boot_animation: {
            value_data[0] = eepdata.display_bootanim;
            break;
        }
		case id_display_timeout: {
            value_data[0] = (eepdata.display_timeout / DISPLAY_TIMEOUT_STEP) - 1;
			break;
		}
    }
}

void via_config_save(void) {
	eeprom_update_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
}
