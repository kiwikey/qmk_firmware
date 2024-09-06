#include "quantum.h"

#include "via_custom.h"
#include "qp_widget_rgbstat.h"

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
	qp_widget_rgbstat_flag = true;
	// oled_on();
	// key_timer = timer_read32(); // turn on OLED
	
    // data = [ command_id, channel_id, value_id, value_data ]
    // uint8_t *command_id        = &(data[0]);
    // uint8_t *channel_id        = &(data[1]);
    // uint8_t *value_id_and_data = &(data[2]);

    // if ( *channel_id == id_custom_channel ) { // id_custom_channel = 0
        // switch ( *command_id ) {
            // case id_custom_set_value: { // id_custom_set_value = 0x07
                // via_config_set_value(value_id_and_data);
                // break;
            // }
            // case id_custom_get_value: { // id_custom_get_value = 0x08
                // via_config_get_value(value_id_and_data);
                // break;
            // }
            // case id_custom_save: { // id_custom_save = 0x09
                // via_config_save();
                // break;
            // }
            // default: { // Unhandled message.
                // *command_id = id_unhandled;
                // break;
            // }
        // }
        // return;
    // }
    // Return the unhandled state
    // *command_id = id_unhandled;

    // DO NOT call raw_hid_send(data,length) here, let caller do this
}
