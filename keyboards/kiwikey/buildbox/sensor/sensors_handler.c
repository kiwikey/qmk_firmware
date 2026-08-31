#include "sensors_handler.h"

#include <lib/lib8tion/lib8tion.h>

#include "features/eeprom_custom.h"
#include "features/knob_custom.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"
#include "display/widgets/qp_menu.h"
#include "display/widgets/qp_widget_breakout.h"
#include "display/widgets/tutorial.h"

int16_t accumulator = 0;
uint32_t last_knob_movement_time = 0;

void keyboard_post_init_sensors_handler(void) {
	keyboard_post_init_magnetic_encoder();
}

void housekeeping_task_sensors_handler(void) {
	// -1 = not yet synced with the sensor's actual boot-time state
	static int8_t magnet_was_present = -1;

	housekeeping_task_magnetic_encoder();

	if (magnet_was_present != (int8_t)magnetic_encoder.is_present) {
		magnet_was_present = magnetic_encoder.is_present;
		// The knob widget is only visible on the idle screen; menu/breakout/
		// tutorial already redraw it correctly (via ui_refresh -> widget_knob_init,
		// or don't show it at all) when they exit, so skip poking the display
		// while they're active.
		if (menu_state == NOT_IN_MENU && !breakout_is_active() && !tutorial_is_active()) {
			magnet_was_present ? widget_knob_show_dot() : widget_knob_show_missing();
		}
	}
}

// Not in any public header, but has external linkage in quantum/keyboard.c.
// Our knob bypasses QMK's ENCODER_ENABLE pipeline entirely (custom AS5600 I2C
// read), so nothing else marks rotation as "activity" for Quantum Painter's
// auto-sleep timer (QUANTUM_PAINTER_DISPLAY_TIMEOUT) unless we do it ourselves.
extern void last_encoder_activity_trigger(void);

// Called from process_magnetic_encoder() (as5600.c) once a movement past
// DEG_MARGIN_AS5600 has already been read and validated. `direction` is
// the sign of that movement: true = CW, false = CCW. This function does
// NOT touch the sensor itself, avoiding a second, racy I2C read per tick.
void magnetic_encoder_update_kb(bool direction) {
	last_encoder_activity_trigger();
	last_knob_movement_time = timer_read32();

	uint16_t distance = get_distance(&magnetic_encoder);

	accumulator += direction ? (int16_t)distance : -(int16_t)distance;

#ifdef CONSOLE_ENABLE
	// printf("accumulator = %d \n", accumulator);
#endif

    if (breakout_is_active()) { // While in game
        while (accumulator >= BREAKOUT_STEP_SIZE) {
            breakout_encoder_tick(CW);
            accumulator -= BREAKOUT_STEP_SIZE;
        }
        while (accumulator <= -BREAKOUT_STEP_SIZE) {
            breakout_encoder_tick(CCW);
            accumulator += BREAKOUT_STEP_SIZE;
        }
    } else if (tutorial_is_active()) {
        // Screen navigation is buttons-only, so this never falls through to
        // mouse-wheel/volume/etc below - but the knob screen shows a live dot
        // that should still track real rotation (tutorial_knob_rotated() is a
        // no-op on every other screen).
        tutorial_knob_rotated();
    } else if (menu_state == NOT_IN_MENU) { // While in main screen
        widget_knob_update(magnetic_encoder.prev_angle, magnetic_encoder.new_angle);

        uint16_t code_cw = KC_NO, code_ccw = KC_NO;
        switch (eepdata.knob_func) {
            case KNOB_FUNC_HSCROLL: code_cw = MS_WHLR; code_ccw = MS_WHLL; break;
            case KNOB_FUNC_VSCROLL: code_cw = MS_WHLD; code_ccw = MS_WHLU; break;
            case KNOB_FUNC_VOLUME:  code_cw = KC_VOLU; code_ccw = KC_VOLD; break;
            default: break; // KNOB_FUNC_CUSTOM - no built-in action yet
        }

        uint16_t sensitivity_threshold = knob_sensitivity_step[eepdata.knob_sensitivity < KNOB_SENSITIVITY_COUNT ? eepdata.knob_sensitivity : KNOB_SENSITIVITY_MEDIUM];
        while (accumulator >= sensitivity_threshold) {
            if (code_cw != KC_NO) tap_code16(code_cw);
            accumulator -= sensitivity_threshold;
        }
        while (accumulator <= -sensitivity_threshold) {
            if (code_ccw != KC_NO) tap_code16(code_ccw);
            accumulator += sensitivity_threshold;
        }
    } else if (debug_screen_is_active()) {
        // Debug is a static info dump (menu_state stays MAIN_MENU while it's
        // shown, since MENU_DEBUG never enters SUB_MENU) - ignore rotation
        // instead of letting it fall through to menu list navigation below.
    } else if (menu_state == MAIN_MENU || menu_state == SUB_MENU) { // While in Menu
        while (accumulator >= MENU_STEP_SIZE) {
            process_encoder_rotate(CW);
            accumulator -= MENU_STEP_SIZE;
        }
        while (accumulator <= -MENU_STEP_SIZE) {
            process_encoder_rotate(CCW);
            accumulator += MENU_STEP_SIZE;
        }
    }
}

bool process_encoder_rotate(bool clockwise) { // Rotating only, no Pressing

	/*** ENCODER IN MENU ***/
	if (menu_state != NOT_IN_MENU) {
		/* In Main-menu, knob rotation controls cursor Up/Down */
		if (menu_state == MAIN_MENU) {
			if (clockwise) { // Turn clockwise => DOWN
				menu_cursor++;
				if (menu_cursor == MENU_LINESPERPAGE+1) // when jumping to next page, re-print the list
					menu_printlist();
			} else {         // Turn counter-clockwise => UP
				menu_cursor--;
				if (menu_cursor == MENU_LINESPERPAGE)
					menu_printlist();
			}
			if (menu_cursor > MENU_MAXITEMS) {
				menu_cursor = 1;             // scroll back to #1
				menu_printlist();            // refresh the list
			}
			if (menu_cursor == 0) {
				menu_cursor = MENU_MAXITEMS; // scroll to last item
				menu_printlist();            // refresh the list
			}
			menu_set_cursor(menu_cursor);
		/* In Sub-menu, knob rotation moves between options */
		/* also note: menu lines that "ischangeable = FALSE" will not run into Sub-menu */
		} else if (menu_state == SUB_MENU) {
			bool value_changed = false;
			switch (menu_cursor) {
				case MENU_DISPLAY_BRIGHTNESS: // DONE
					if (clockwise) { // next
						if (eepdata.display_brightness == BACKLIGHT_LEVELS)
							eepdata.display_brightness = 1;
						else eepdata.display_brightness++;
						backlight_level(eepdata.display_brightness);
					} else {         // previous
						if (eepdata.display_brightness == 1)
							eepdata.display_brightness = BACKLIGHT_LEVELS;
						else eepdata.display_brightness--;
						backlight_level(eepdata.display_brightness);
					}
					value_changed = true;
					break;
				case MENU_RGB_BRIGHTNESS: // DONE
					if (clockwise) { // next
						rgb_matrix_increase_val();
					} else {         // previous
						rgb_matrix_decrease_val();
					}
					value_changed = true;
					break;
				case MENU_RGB_MODE: // DONE
					if (clockwise) { // next
						rgb_matrix_step();
					} else {         // previous
						rgb_matrix_step_reverse();
					}
					value_changed = true;
					break;
				case MENU_KNOB_RGB:
					if (clockwise) { // next
						eepdata.knob_effect = (eepdata.knob_effect == KNOB_EFFECT_LAYER) ? KNOB_EFFECT_OFF : eepdata.knob_effect + 1;
					} else {         // previous
						eepdata.knob_effect = (eepdata.knob_effect == KNOB_EFFECT_OFF) ? KNOB_EFFECT_LAYER : eepdata.knob_effect - 1;
					}
					value_changed = true;
					break;
				case MENU_ANIMATION: // DONE
					eepdata.display_bootanim ^= 1;
					value_changed = true;
					break;
				case MENU_DISPLAYTIMEOUT:
					if (clockwise) { // next
						eepdata.display_timeout += DISPLAY_TIMEOUT_STEP;
						if (eepdata.display_timeout > DISPLAY_TIMEOUT_NEVER)
							eepdata.display_timeout = DISPLAY_TIMEOUT_MIN;
					} else {         // previous
						eepdata.display_timeout -= DISPLAY_TIMEOUT_STEP;
						if (eepdata.display_timeout <= 0)
							eepdata.display_timeout = DISPLAY_TIMEOUT_NEVER;
					}
					value_changed = true;
					break;
				case MENU_KNOB_FUNC:
					if (clockwise) { // next
						eepdata.knob_func = (eepdata.knob_func == KNOB_FUNC_CUSTOM) ? KNOB_FUNC_HSCROLL : eepdata.knob_func + 1;
					} else {         // previous
						eepdata.knob_func = (eepdata.knob_func == KNOB_FUNC_HSCROLL) ? KNOB_FUNC_CUSTOM : eepdata.knob_func - 1;
					}
					value_changed = true;
					break;
				case MENU_THEME_COLOR:
					// uint8_t overflow wraps 0..255 cleanly - hue is circular, so no clamping needed
					eepdata.theme_hue += clockwise ? THEME_COLOR_HUE_STEP : -THEME_COLOR_HUE_STEP;
					value_changed = true;
					break;
				case MENU_KNOB_SENSITIVITY:
					// 3 fixed levels (LOW/MEDIUM/HIGH), cycled the same way as MENU_KNOB_FUNC
					if (clockwise) { // next (less sensitive -> more sensitive)
						eepdata.knob_sensitivity = (eepdata.knob_sensitivity == KNOB_SENSITIVITY_HIGH) ? KNOB_SENSITIVITY_LOW : eepdata.knob_sensitivity + 1;
					} else {         // previous
						eepdata.knob_sensitivity = (eepdata.knob_sensitivity == KNOB_SENSITIVITY_LOW) ? KNOB_SENSITIVITY_HIGH : eepdata.knob_sensitivity - 1;
					}
					value_changed = true;
					break;
				default:
					; //
			}
			// TODO: Animation, LCD Timeout, LCD Brightness, Knob Rotation Fn
			if (value_changed) {
				menu_render_sidebar(menu_cursor, (menu_cursor - 1) % MENU_LINESPERPAGE);
				qp_flush(my_display);
			}
		}
		return false;
	}
    return true;
}