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

void keyboard_post_init_sensors_handler(void) {
	keyboard_post_init_magnetic_encoder();
}

void housekeeping_task_sensors_handler(void) {
	housekeeping_task_magnetic_encoder();
}

// Called from process_magnetic_encoder() (as5600.c) once a movement past
// DEG_MARGIN_AS5600 has already been read and validated. `direction` is
// the sign of that movement: true = CW, false = CCW. This function does
// NOT touch the sensor itself, avoiding a second, racy I2C read per tick.
void magnetic_encoder_update_kb(bool direction) {
	uint16_t distance = get_distance(&magnetic_encoder);

	accumulator += direction ? (int16_t)distance : -(int16_t)distance;

#ifdef CONSOLE_ENABLE
	// printf("accumulator = %d \n", accumulator);
#endif

    if (breakout_is_active()) {
        while (accumulator >= BREAKOUT_STEP_SIZE) {
            breakout_move_paddle(CW);
            accumulator -= BREAKOUT_STEP_SIZE;
        }
        while (accumulator <= -BREAKOUT_STEP_SIZE) {
            breakout_move_paddle(CCW);
            accumulator += BREAKOUT_STEP_SIZE;
        }
    } else if (menu_state == NOT_IN_MENU) {
        widget_knob_update(magnetic_encoder.prev_angle, magnetic_encoder.new_angle);
        while (accumulator >= STEP_SIZE) {
            tap_code16(dynamic_keymap_get_keycode(0, 0, 0));
            accumulator -= STEP_SIZE;
        }
        while (accumulator <= -STEP_SIZE) {
            tap_code16(dynamic_keymap_get_keycode(0, 0, 1));
            accumulator += STEP_SIZE;
        }
    } else if (menu_state == MAIN_MENU) {
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
				if (menu_cursor == MAINMENU_LINESPERPAGE+1) // when jumping to next page, re-print the list
					menu_printlist();
			} else {         // Turn counter-clockwise => UP
				menu_cursor--;
				if (menu_cursor == MAINMENU_LINESPERPAGE)
					menu_printlist();
			}
			if (menu_cursor > MAINMENU_MAXITEMS) {
				menu_cursor = 1;                 // scroll back to #1
				menu_printlist();                // refresh the list
			}
			if (menu_cursor == 0) {
				menu_cursor = MAINMENU_MAXITEMS; // scroll to last item
				menu_printlist();                // refresh the list
			}
			menu_set_cursor(menu_cursor);
		/* In Sub-menu, knob rotation moves between options */
		/* also note: menu lines that "ischangeable = FALSE" will not runs into Sub-menu */
		} else if (menu_state == SUB_MENU) {
			// if (clockwise) { // Turn clockwise => RIGHT
			// 	if (menu_cursor == MENU_ACTIVATELAYER) {
			// 		if (eepdata.active_layer == DYNAMIC_KEYMAP_LAYER_COUNT-1) eepdata.active_layer = 0;
			// 		else eepdata.active_layer++;
			// 	}
			// 	if (menu_cursor == MENU_ANIMATION) {
			// 		if (eepdata.display_anim == DISPLAY_ANIM_QTY) eepdata.display_anim = 0;
			// 		else eepdata.display_anim++;
			// 	}
			// 	if (menu_cursor == MENU_DISPLAYTIMEOUT) {
			// 		eepdata.display_timeout += DISPLAY_TIMEOUT_STEP;
			// 		if (eepdata.display_timeout > DISPLAY_TIMEOUT_NEVER)
			// 			eepdata.display_timeout = DISPLAY_TIMEOUT_MIN;
			// 	}
			// 	#if defined(QUANTUM_PAINTER_ENABLE)
			// 	if (menu_cursor == MENU_DISPLAYBRIGHTNESS) {
			// 		if (eepdata.display_brightness == BACKLIGHT_LEVELS)
			// 			eepdata.display_brightness = 1;
			// 		else eepdata.display_brightness++;
			// 		backlight_level(eepdata.display_brightness);
			// 	}
			// 	if (menu_cursor == MENU_DISPLAYROTATION) {
			// 		if (eepdata.display_rotation == QP_ROTATION_90) // or QP_ROTATION_270
			// 			eepdata.display_rotation = QP_ROTATION_0;
			// 		else eepdata.display_rotation++;
			// 		display_rotate_flag = true;
			// 	}
			// 	#endif // defined(QUANTUM_PAINTER_ENABLE)
			// 	// if (menu_cursor == MENU_LIGHTINGLAYERS) {
			// 		// if (eepdata.lighting_flags == 2) {
			// 			// eepdata.lighting_flags = 0;
			// 			// eepdata.lighting_layers = 0;
			// 		// } else eepdata.lighting_layers ? eepdata.lighting_flags++ : eepdata.lighting_layers++; // Drawing a flowchart may help
			// 		// if (eepdata.lighting_flags == 2) {   // if Lighting Layer is enabled for ALL LEDS
			// 			// eepdata.lighting_flags = 0;      // ???
			// 			// eepdata.lighting_layers = false; // LL off
			// 		// } else eepdata.lighting_layers ? eepdata.lighting_flags++ : (eepdata.lighting_layers = false); // Drawing a flowchart may help
			// 	// }
			// 	if (menu_cursor == MENU_KNOBFUNCTION) {
			// 		if (eepdata.knob_func == ENCODER_FUNC_MAX) eepdata.knob_func = 0;
			// 		else eepdata.knob_func++;
			// 	}
			// } else {         // Turn counter-clockwise => LEFT
			// 	if (menu_cursor == MENU_ACTIVATELAYER) {
			// 		if (eepdata.active_layer == 0) eepdata.active_layer = DYNAMIC_KEYMAP_LAYER_COUNT-1;
			// 		else eepdata.active_layer--;
			// 	}
			// 	if (menu_cursor == MENU_ANIMATION) {
			// 		if (eepdata.display_anim == 0) eepdata.display_anim = DISPLAY_ANIM_QTY;
			// 		else eepdata.display_anim--;
			// 	}
			// 	if (menu_cursor == MENU_DISPLAYTIMEOUT) {
			// 		eepdata.display_timeout -= DISPLAY_TIMEOUT_STEP;
			// 		if (eepdata.display_timeout <= 0)
			// 			eepdata.display_timeout = DISPLAY_TIMEOUT_NEVER;
			// 	}
			// 	#if defined(QUANTUM_PAINTER_ENABLE)
			// 	if (menu_cursor == MENU_DISPLAYBRIGHTNESS) {
			// 		if (eepdata.display_brightness == 1)
			// 			eepdata.display_brightness = BACKLIGHT_LEVELS;
			// 		else eepdata.display_brightness--;
			// 		backlight_level(eepdata.display_brightness);
			// 	}
			// 	if (menu_cursor == MENU_DISPLAYROTATION) {
			// 		if (eepdata.display_rotation == QP_ROTATION_0)
			// 			eepdata.display_rotation = QP_ROTATION_90; // or QP_ROTATION_270
			// 		else eepdata.display_rotation--;
			// 		display_rotate_flag = true;
			// 		// qp_init(my_display, eepdata.display_rotation);
			// 		// menu_init();
			// 	}
			// 	#endif // defined(QUANTUM_PAINTER_ENABLE)
			// 	// if (menu_cursor == MENU_LIGHTINGLAYERS) {
			// 		// if (eepdata.lighting_layers == 0) {
			// 			// eepdata.lighting_layers = 1;
			// 			// eepdata.lighting_flags = 2;
			// 		// } else eepdata.lighting_flags ? eepdata.lighting_flags-- : (eepdata.lighting_layers = 0);  // Drawing a flowchart may help
			// 	// }
			// 	if (menu_cursor == MENU_KNOBFUNCTION) {
			// 		if (eepdata.knob_func == 0) eepdata.knob_func = ENCODER_FUNC_MAX;
			// 		else eepdata.knob_func--;
			// 	}
			// }
		}
		// menu_quick_view();
		return false;
	}
    return true;
}