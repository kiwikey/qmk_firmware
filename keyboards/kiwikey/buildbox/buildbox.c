#include "buildbox.h"

#include <qp.h>
#include "via.h"
#include "print.h"
#include "sensor/sensors_handler.h"
#include "features/eeprom_custom.h"
#include "features/knob_custom.h"

#if defined(QUANTUM_PAINTER_ENABLE)
	#include "display/defines.h"
	#include "display/qp_graphics.h"
	#include "display/qp_includes.h"
	#include "display/qp_custom_api.h"
	#include "display/widgets/qp_widget_matrix.h"
	#include "display/widgets/qp_widget_layer.h"
	#include "display/widgets/qp_widget_knob.h"
	#include "display/widgets/qp_menu.h"
	#include "display/widgets/qp_widget_breakout.h"
#endif // defined(QUANTUM_PAINTER_ENABLE)

EEPROM_CUSTOM_DATA eepdata;
EEPROM_CUSTOM_DATA eepdata_default = {
	0,                        // Layer 0
	1,                        // Boot animation enabled
	DISPLAY_TIMEOUT_MIN,      // LCD Timeout 30s
	BACKLIGHT_DEFAULT_LEVEL,  // LCD Brightness default (10 = max)
	0,                        // Lighting Layers OFF
	0,                        // Lighting Layers applied to Knob's LEDs
	{ 126, 210,  42,  84 },   // Lighting Layers' HUEs: Cyan - Magenta - Yellow - Green
	{ 255, 255, 255, 255 },   // Lighting Layers' SATs: maximum (255)
	1,                        // Knob special effect enabled
	1,                        // Knob: Volume
	7                         // Checksum is always 7
};

void keyboard_post_init_kb(void) {
	// Reading all EEPROM custom datas, refer to 'eeprom_custom.h' for detail
    eeprom_read_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));

	/*** Validation check ***/
	/* This runs everytime the EEPROM is corrupted, or right after 'factory_reset' or 'bootmagic_reset' */
	if (eepdata.checksum != 7) {
		eeprom_update_block(&eepdata_default, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
		// Reading all EEPROM custom datas, again
		eeprom_read_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
	}

	layer_move(eepdata.active_layer);
	#if defined(BACKLIGHT_ENABLE)
		backlight_enable(); // TFT backlight
		// backlight_level(eepdata.display_brightness);
		backlight_level(10);
	#endif // defined(BACKLIGHT_ENABLE)

	keyboard_post_init_sensors_handler();
	keyboard_post_init_display();  // eepdata.display_bootanim is checked here
	keyboard_post_init_user();
}

void housekeeping_task_kb(void) {
	if (!booting) {
		housekeeping_task_display();
		housekeeping_task_sensors_handler();
		housekeeping_task_breakout();
	}
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	if (!process_record_display(keycode, record)) {
		return false;
	}
	return process_record_user(keycode, record);
}

layer_state_t layer_state_set_kb(layer_state_t state) {
	if (!booting) {
		widget_layer_render(get_highest_layer(state));
		widget_matrix_keymap_render(get_highest_layer(state));
	}
	return state;
}

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) { // Lighting Layers
    if (!rgb_matrix_indicators_advanced_user(led_min, led_max)) {
        return false;
    }

	HSV hsv = {
		eepdata.layer_hue[get_highest_layer(layer_state)],
		eepdata.layer_sat[get_highest_layer(layer_state)],
		rgb_matrix_get_val() // VAL = current RGBMatrix's brightness
	};
	if (hsv.s == 0) hsv.v = 0;
    RGB rgb = hsv_to_rgb(hsv);

	if (eepdata.lighting_layers == 0) // If Lighting Layers is off, there's nothing to do here
		return false;

    for (uint8_t i = led_min; i < led_max; i++) {
		dprintf("g_led_config.flags[%d] = %d",i, g_led_config.flags[i]);
		// if (g_led_config.flags[i] & ((eepdata.lighting_flags+1) << 1)) {
		if (g_led_config.flags[i] == LED_FLAG_INDICATOR) {
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }

	knob_effect();

    return false;
}

void suspend_power_down_kb(void) {
    qp_power(my_display, false);
}

void suspend_wakeup_init_kb(void) {
    qp_power(my_display, true);
}