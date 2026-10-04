#include "via_custom.h"
#include "raw_hid.h"
#include "eeprom_custom.h"
#include "display/qp_graphics.h"
#include "webhid_stream.h"

#if defined(QUANTUM_PAINTER_ENABLE)
	#include "display/widgets/qp_menu.h"
	#include "display/widgets/qp_widget_breakout.h"
	#include "display/widgets/tutorial.h"
	#include "display/widgets/qp_widget_screensaver.h"

// VIA's color picker streams a set_value packet per drag tick - repainting the
// whole idle screen on every one of those would make the TFT stutter/flicker
// continuously while the user is still dragging. Debounce with a trailing
// edge instead: each incoming packet pushes this deferred repaint 1s further
// into the future, so it only actually fires once, using the latest hue,
// after packets stop arriving for a full second.
#define THEME_COLOR_REPAINT_DEBOUNCE_MS 1000
static deferred_token theme_color_repaint_token = INVALID_DEFERRED_TOKEN;

static uint32_t theme_color_repaint_callback(uint32_t trigger_time, void *cb_arg) {
	theme_color_repaint_token = INVALID_DEFERRED_TOKEN;
	// Only repaint while the idle screen is showing - whatever covers it (menu,
	// Breakout, tutorial, screensaver) calls ui_refresh() itself when it closes,
	// which picks up the new hue then.
	if (ui_idle_screen_visible()) {
		ui_refresh();
	}
	return 0; // one-shot, don't requeue
}
#endif // defined(QUANTUM_PAINTER_ENABLE)

// Not in any public header, but has external linkage in quantum/keyboard.c
// (same as last_encoder_activity_trigger() in sensor/sensors_handler.c).
extern void last_matrix_activity_trigger(void);

// A change made from VIA counts as the user using the board: restarts the LCD
// Timeout / screensaver idle countdown (and wakes the backlight - see
// housekeeping_task_display()), and closes a running screensaver.
static void via_mark_user_activity(void) {
	last_matrix_activity_trigger();
#if defined(QUANTUM_PAINTER_ENABLE)
	if (screensaver_is_active()) {
		screensaver_exit();
	}
#endif // defined(QUANTUM_PAINTER_ENABLE)
}

/* via_custom_value_command_kb dispatches by channel_id:
	 id_custom_channel (0)            - this keyboard's own config, handled inline below
	 WEBHID_CONFIG_CHANNEL_ID (0x20)  - forwarded to webhid_stream_handle_config()
   Only these command_ids are handled on either channel:
		id_custom_set_value                     = 0x07,
		id_custom_get_value                     = 0x08,
		id_custom_save                          = 0x09,
*/
void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
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
				eeprom_custom_save();
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
	if ( *channel_id == WEBHID_CONFIG_CHANNEL_ID ) { // = 0x20, see features/webhid_stream.h
		webhid_stream_handle_config(*command_id, value_id_and_data);
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
		// Only one pending single-key redraw fits in the flag - if the previous one
		// hasn't been drawn yet, fall back to redrawing the whole grid
		if (flag_display_keycode_changed & 0x1000) {
			flag_display_keymap_reload = true;
		}
		flag_display_keycode_changed = ( 0x1000 | (command_data[0]<<8) | (command_data[1]<<4) | command_data[2]);
	} else if (*command_id == id_dynamic_keymap_set_buffer || *command_id == id_dynamic_keymap_reset || *command_id == id_eeprom_reset) {
		// Many keys at once (VIA "Load saved layout", keymap reset) - redraw the whole grid
		flag_display_keymap_reload = true;
	}

	// Only commands that *change* something count as activity - VIA's periodic
	// reads don't, and neither does the web mirror's PING (WEBHID_CONFIG_CHANNEL_ID),
	// or an open mirror tab would keep the display awake forever.
	switch (*command_id) {
		case id_dynamic_keymap_set_keycode:
		case id_dynamic_keymap_reset:
		case id_dynamic_keymap_set_buffer:
		case id_dynamic_keymap_macro_set_buffer:
		case id_dynamic_keymap_macro_reset:
		case id_custom_save:
			via_mark_user_activity();
			break;
		case id_custom_set_value:
			if (data[1] != WEBHID_CONFIG_CHANNEL_ID) {
				via_mark_user_activity();
			}
			break;
		default:
			break;
	}

	// The knob's KLE legend has an 'ei' (encoder index) so VIA draws it as a round
	// knob cap instead of a square key - but the knob bypasses QMK's ENCODER_MAP
	// feature entirely (custom AS5600 sensor, see sensor/sensors_handler.c), so
	// ENCODER_MAP_ENABLE is never defined and via.c's own get/set_encoder cases are
	// compiled out, replying id_unhandled. That breaks "Save Current Layout" in VIA:
	// save-load.tsx awaits getEncoderValue() for this key before writing the file,
	// and the rejected promise aborts the save with an already-created but empty
	// file. Answer both commands here so VIA gets a normal response; the knob's
	// actual behavior is controlled by id_knob_func, not per-layer keycodes.
	if (*command_id == id_dynamic_keymap_get_encoder) {
		command_data[3] = 0; // KC_NO
		command_data[4] = 0;
		raw_hid_send(data, length); // via_command_kb() must send the reply itself when it returns true
		return true;
	}
	if (*command_id == id_dynamic_keymap_set_encoder) {
		raw_hid_send(data, length); // no-op, nothing to persist - just ack it
		return true;
	}

#if defined(BACKLIGHT_ENABLE)
	if (*command_id == id_custom_set_value && data[1] == id_qmk_backlight_channel && data[2] == id_qmk_backlight_brightness) {
		uint8_t level = ((uint16_t)data[3] * BACKLIGHT_LEVELS) / UINT8_MAX;
		eepdata.display_brightness = (level == 0) ? 1 : level;
	} else if (*command_id == id_custom_save && data[1] == id_qmk_backlight_channel) {
		eeprom_custom_save(); // this channel never reaches via_custom_value_command_kb(), so the usual save path can't run
	}
#endif // defined(BACKLIGHT_ENABLE)

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
			if (*value_data >= DYNAMIC_KEYMAP_LAYER_COUNT) break; // out of range - ignore, every per-layer array is sized by this
			eepdata.active_layer = *value_data;
			layer_move(eepdata.active_layer); // This triggers layer_state_set_kb()
			break;
		}
		case id_rgb_layers_enable: {
			eepdata.lighting_layers = *value_data;
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
			if (data[1] >= DYNAMIC_KEYMAP_LAYER_COUNT) break; // out of range - would write past layer_hue[]/layer_sat[]
			eepdata.layer_hue[data[1]] = data[2];
			eepdata.layer_sat[data[1]] = data[3];
			flag_widget_layer_changed = data[1] + 1; // flag need to +1, see 'flag_widget_layer_changed' definition in qp_graphics.c
			break;
		}

		// KNOB CONFIGS
		case id_rgb_wheel: {
			eepdata.knob_effect = *value_data;
			break;
		}
		case id_knob_func: {
			eepdata.knob_func = *value_data;
			break;
		}
		case id_knob_sensitivity: {
			eepdata.knob_sensitivity = *value_data;
			break;
		}

		// LCD CONFIGS
		case id_boot_animation: {
			eepdata.display_bootanim = *value_data;
			break;
		}
		case id_display_timeout: {
			// eepdata.display_timeout is an index into display_timeout_seconds[]/
			// display_timeout_text[] (qp_graphics.h) - VIA's dropdown sends that
			// index directly, same as id_knob_func/id_knob_sensitivity above.
			eepdata.display_timeout = *value_data;
			break;
		}
		case id_theme_color: {
			// "color" type sends [hue, sat]; only hue is stored - GLOBAL_THEME_COLOR
			// (display/defines.h) keeps sat fixed at 255, there's no theme_sat field
			eepdata.theme_hue = data[1];
#if defined(QUANTUM_PAINTER_ENABLE)
			// See theme_color_repaint_callback() above: debounced, not immediate.
			if (theme_color_repaint_token == INVALID_DEFERRED_TOKEN ||
				!extend_deferred_exec(theme_color_repaint_token, THEME_COLOR_REPAINT_DEBOUNCE_MS)) {
				theme_color_repaint_token = defer_exec(THEME_COLOR_REPAINT_DEBOUNCE_MS, theme_color_repaint_callback, NULL);
			}
#endif // defined(QUANTUM_PAINTER_ENABLE)
			break;
		}

		// SYSTEM CONTROL
		case id_soft_reset: {
			soft_reset_keyboard();
			break;
		}
		case id_reset_to_dfu: {
			action_resettodfu();
			break;
		}
	}
	eeprom_custom_validate(); // a value VIA sent that's out of range falls back to its default
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
		case id_rgb_layers_hue: { // going to send an array
			// value_data[0] is array index
			// value_data[1] is value of HUE
			// value_data[2] is value of SAT
			value_data[0] = data[1];
			if (data[1] >= DYNAMIC_KEYMAP_LAYER_COUNT) { // out of range - reply 0/0 instead of reading past the arrays
				value_data[1] = 0;
				value_data[2] = 0;
				break;
			}
			value_data[1] = eepdata.layer_hue[data[1]];
			value_data[2] = eepdata.layer_sat[data[1]];
			break;
		}

		// KNOB CONFIGS
		case id_rgb_wheel: {
			value_data[0] = eepdata.knob_effect;
			break;
		}
		case id_knob_func: {
			value_data[0] = eepdata.knob_func;
			break;
		}
		case id_knob_sensitivity: {
			value_data[0] = eepdata.knob_sensitivity;
			break;
		}

		// LCD CONFIGS
		case id_boot_animation: {
			value_data[0] = eepdata.display_bootanim;
			break;
		}
		case id_display_timeout: {
			value_data[0] = eepdata.display_timeout;
			break;
		}
		case id_theme_color: {
			value_data[0] = eepdata.theme_hue;
			value_data[1] = 255; // sat is fixed, matches GLOBAL_THEME_COLOR (display/defines.h)
			break;
		}
	}
}
