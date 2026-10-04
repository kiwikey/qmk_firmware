#include "buildbox.h"

#include <qp.h>
#include "via.h"
#include "print.h"
#include "sensor/sensors_handler.h"
#include "features/eeprom_custom.h"
#include "features/knob_custom.h"
#include "features/webhid_stream.h"

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
	#include "display/widgets/qp_widget_screensaver.h"
	#include "display/widgets/tutorial.h"
#endif // defined(QUANTUM_PAINTER_ENABLE)

void keyboard_post_init_kb(void) {

	eeprom_custom_load(); // BuildBox's own settings (eepdata) - refer to 'eeprom_custom.h' for detail

	// Runs before the display exists - layer_state_set_kb() skips drawing until
	// display_ready, and ui_refresh() later draws this layer from the start.
	layer_move(eepdata.active_layer);

	keyboard_post_init_sensors_handler();
	keyboard_post_init_display();

	#if defined(BACKLIGHT_ENABLE)
		backlight_enable(); // TFT backlight - after display init, so the panel is already showing a clean frame
		// eepdata.display_brightness is the one saved copy of the LCD brightness -
		// every backlight call in this firmware is _noeeprom, so QMK's own backlight
		// EEPROM level (only written by VIA's built-in brightness "save") is just
		// overridden here at boot.
		backlight_level_noeeprom(eepdata.display_brightness);
	#endif // defined(BACKLIGHT_ENABLE)

	keyboard_post_init_webhid_stream(); // no-op stub unless WEBHID_STREAM_ENABLE (webhid_stream.h)
	keyboard_post_init_user();
}

void housekeeping_task_kb(void) {
	if (ui_get_mode() != UI_MODE_BOOT) { // all housekeeping waits for the boot animation
		housekeeping_task_display();
		housekeeping_task_sensors_handler();
		housekeeping_task_breakout();
		housekeeping_task_screensaver();
		housekeeping_task_webhid_stream();
	}
	housekeeping_task_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	if (!process_record_display(keycode, record)) {
		return false;
	}
	if (!process_record_user(keycode, record)) {
		return false;
	}

	// Shortcut keycodes offered in VIA's "customKeycodes" (buildbox_via.json) - sent on press
	uint16_t shortcut = KC_NO;
	switch (keycode) {
		case KC_WIN_CUT:     shortcut = LCTL(KC_X);       break;
		case KC_WIN_COPY:    shortcut = LCTL(KC_C);       break;
		case KC_WIN_PASTE:   shortcut = LCTL(KC_V);       break;
		case KC_WIN_DESKTOP: shortcut = LGUI(KC_D);       break;
		case KC_WIN_SNIP:    shortcut = LGUI(LSFT(KC_S)); break;
		case KC_MAC_CUT:     shortcut = LGUI(KC_X);       break;
		case KC_MAC_COPY:    shortcut = LGUI(KC_C);       break;
		case KC_MAC_PASTE:   shortcut = LGUI(KC_V);       break;
		default:             return true;
	}
	if (record->event.pressed) {
		tap_code16(shortcut);
	}
	return false;
}

layer_state_t layer_state_set_kb(layer_state_t state) {
	state = layer_state_set_user(state);

	// Only redraw the layer/matrix widgets while the idle screen is actually
	// showing (see ui_idle_screen_visible(), qp_graphics.c) - e.g. VIA's "Active
	// Layer" dropdown can change the layer while the menu/Breakout/tutorial/
	// screensaver is up, and must not draw over it. Also false before the display
	// exists (keyboard_post_init_kb()'s layer_move() runs before display init).
	uint8_t layer = get_highest_layer(state);
	if (ui_idle_screen_visible() && layer < DYNAMIC_KEYMAP_LAYER_COUNT) {
		widget_layer_render_layername(layer, WIDGET_LAYER_POSX, WIDGET_LAYER_POSY);
		widget_layer_render_navigation(layer);
		widget_matrix_keymap_render(layer);
	}
	return state;
}

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) { // Lighting Layers
	if (!rgb_matrix_indicators_advanced_user(led_min, led_max)) {
		return false;
	}

	uint8_t layer = get_highest_layer(layer_state|default_layer_state); // same layer pick as knob_effect() (knob_custom.c)
	if (eepdata.lighting_layers != 0 && layer < DYNAMIC_KEYMAP_LAYER_COUNT) { // If Lighting Layers is off, there's nothing to do here
		HSV hsv = {
			eepdata.layer_hue[layer],
			eepdata.layer_sat[layer],
			rgb_matrix_get_val() // VAL = current RGBMatrix's brightness
		};
		if (hsv.s == 0) hsv.v = 0;
		RGB rgb = hsv_to_rgb(hsv);

		for (uint8_t i = led_min; i < led_max; i++) {
			if (HAS_FLAGS(g_led_config.flags[i], LED_FLAG_INDICATOR)) {
				rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
			}
		}
	}

	knob_effect(); // independent of Lighting Layers; runs last so it wins on the ring LEDs

	return false;
}

void suspend_power_down_kb(void) {
	qp_power(bb_display, false);
	suspend_power_down_user();
}

void suspend_wakeup_init_kb(void) {
	qp_power(bb_display, true);
	#if defined(BACKLIGHT_ENABLE)
		// QMK's own resume (suspend_wakeup_init_quantum()) has just restored the
		// backlight from its saved level - re-apply ours, so the panel stays dark
		// if the LCD Timeout had already put it to sleep (display_is_asleep()).
		backlight_level_noeeprom(display_is_asleep() ? 0 : eepdata.display_brightness);
	#endif // defined(BACKLIGHT_ENABLE)
	suspend_wakeup_init_user();
}