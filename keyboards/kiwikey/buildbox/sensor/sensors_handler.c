#include "sensors_handler.h"

#include <lib/lib8tion/lib8tion.h>

#include "features/eeprom_custom.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"
#include "display/widgets/qp_menu.h"
#include "display/widgets/qp_widget_breakout.h"

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
		// The knob widget is only visible on the idle screen; menu/breakout
		// already redraw it correctly (via ui_refresh -> widget_knob_init)
		// when they exit, so skip poking the display while they're active.
		if (menu_state == NOT_IN_MENU && !breakout_is_active()) {
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

    if (breakout_is_active()) {
        while (accumulator >= BREAKOUT_STEP_SIZE) {
            breakout_encoder_tick(CW);
            accumulator -= BREAKOUT_STEP_SIZE;
        }
        while (accumulator <= -BREAKOUT_STEP_SIZE) {
            breakout_encoder_tick(CCW);
            accumulator += BREAKOUT_STEP_SIZE;
        }
    } else if (menu_state == NOT_IN_MENU) {
        widget_knob_update(magnetic_encoder.prev_angle, magnetic_encoder.new_angle);
        while (accumulator >= STEP_SIZE) {
            tap_code16(MS_WHLU);
            accumulator -= STEP_SIZE;
        }
        while (accumulator <= -STEP_SIZE) {
            tap_code16(MS_WHLD);
            accumulator += STEP_SIZE;
        }
    } else if (menu_state == MAIN_MENU || menu_state == SUB_MENU) {
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
				menu_cursor = 1;                 // scroll back to #1
				menu_printlist();                // refresh the list
			}
			if (menu_cursor == 0) {
				menu_cursor = MENU_MAXITEMS; // scroll to last item
				menu_printlist();                // refresh the list
			}
			menu_set_cursor(menu_cursor);
		/* In Sub-menu, knob rotation moves between options */
		/* also note: menu lines that "ischangeable = FALSE" will not runs into Sub-menu */
		} else if (menu_state == SUB_MENU) {
			bool value_changed = false;
			if (menu_cursor == MENU_ACTIVATELAYER) {
				if (clockwise) { // next
					eepdata.active_layer = (eepdata.active_layer == DYNAMIC_KEYMAP_LAYER_COUNT-1) ? 0 : eepdata.active_layer+1;
				} else {          // previous
					eepdata.active_layer = (eepdata.active_layer == 0) ? DYNAMIC_KEYMAP_LAYER_COUNT-1 : eepdata.active_layer-1;
				}
				value_changed = true;
			}
			// TODO: Animation, LCD Timeout, LCD Brightness, Knob Rotation Fn
			if (value_changed) {
				menu_render_sidebar(menu_cursor, (menu_cursor - 1) % MENU_LINESPERPAGE);
				qp_flush(my_display);
			}
		}
		// menu_quick_view();
		return false;
	}
    return true;
}