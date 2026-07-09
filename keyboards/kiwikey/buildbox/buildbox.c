#include "buildbox.h"

#include <qp.h>
#include "via.h"
#include "print.h"
#include "sensor/sensors_handler.h"

#if defined(QUANTUM_PAINTER_ENABLE)
	#include "display/defines.h"
	#include "display/qp_graphics.h"
	#include "display/qp_includes.h"
	#include "display/qp_custom_api.h"
	#include "display/eeprom_custom.h"
	#include "display/widgets/qp_widget_matrix.h"
	#include "display/widgets/qp_widget_layer.h"
	#include "display/widgets/qp_widget_knob.h"
#endif // defined(QUANTUM_PAINTER_ENABLE)

void keyboard_post_init_kb(void) {
	keyboard_post_init_display();
	// keyboard_post_init_sensors_handler();
	keyboard_post_init_user();
	backlight_enable();
}

void housekeeping_task_kb(void) {
	// housekeeping_task_sensors_handler();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	process_record_display(keycode, record);
	return process_record_user(keycode, record);
}

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {
    for (uint8_t i = 22; i < 30; i++) {
		rgb_matrix_set_color_hsv(i, layer_colors[get_highest_layer(layer_state|default_layer_state)]);
    }
    return false;
}

layer_state_t layer_state_set_kb(layer_state_t state) {
	if (!booting) {
		widget_matrix_bgclear();
		widget_layer_render(get_highest_layer(state));
		widget_matrix_keymap_render(get_highest_layer(state));
	}
	return state;
}

void suspend_power_down_kb(void) {
    // Turn off the display
    qp_power(my_display, false);
}

void suspend_wakeup_init_kb(void) {
    // Turn it back on
    qp_power(my_display, true);
}