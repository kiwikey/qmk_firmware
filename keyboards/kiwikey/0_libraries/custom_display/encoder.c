// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later
 
#include "encoder.h"
#include "eeprom_custom.h"

#if defined(OLED_ENABLE)
	#include "oled.h"
	#include "oled/oled_menu.h"
	#include "oled/oled_ui.h"
#endif // defined(OLED_ENABLE)

#if defined(QUANTUM_PAINTER_ENABLE)
    #include "qp_graphics.h"
	#include "qp/qp_menu.h"
	// #include "qp/qp_ui.h"
	extern painter_device_t my_display;
	extern bool display_rotate_flag;
#endif // defined(QUANTUM_PAINTER_ENABLE)

bool process_encoder_rotate(uint8_t index, bool clockwise) { // Rotating only, no Pressing

	#if defined(QUANTUM_PAINTER_ENABLE)
	/*** ENCODER IN MENU ***/
	if (current_menu != NOT_IN_MENU) {
		/* In Main-menu, knob rotation controls cursor Up/Down */
		if (current_menu == MAIN_MENU) {
			if (clockwise) { // Turn clockwise => DOWN
				menu_cursor++;
				if (menu_cursor > MAINMENU_LINESPERPAGE)
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
		} else if (current_menu == SUB_MENU) {
			if (clockwise) { // Turn clockwise => RIGHT
				if (menu_cursor == MENU_ACTIVATELAYER) {
					if (eepdata.active_layer == DYNAMIC_KEYMAP_LAYER_COUNT-1) eepdata.active_layer = 0;
					else eepdata.active_layer++;
				}
				if (menu_cursor == MENU_ANIMATION) {
					if (eepdata.display_anim == DISPLAY_ANIM_QTY) eepdata.display_anim = 0;
					else eepdata.display_anim++;
				}
				if (menu_cursor == MENU_DISPLAYTIMEOUT) {
					eepdata.display_timeout += DISPLAY_TIMEOUT_STEP;
					if (eepdata.display_timeout > DISPLAY_TIMEOUT_NEVER)
						eepdata.display_timeout = DISPLAY_TIMEOUT_MIN;
				}
				#if defined(QUANTUM_PAINTER_ENABLE)
				if (menu_cursor == MENU_DISPLAYBRIGHTNESS) {
					if (eepdata.display_brightness == BACKLIGHT_LEVELS)
						eepdata.display_brightness = 1;
					else eepdata.display_brightness++;
					backlight_level(eepdata.display_brightness);
				}
				if (menu_cursor == MENU_DISPLAYROTATION) {
					if (eepdata.display_rotation == QP_ROTATION_90) // or QP_ROTATION_270
						eepdata.display_rotation = QP_ROTATION_0;
					else eepdata.display_rotation++;
					display_rotate_flag = true;
				}
				#endif // defined(QUANTUM_PAINTER_ENABLE)
				// if (menu_cursor == MENU_LIGHTINGLAYERS) {
					// if (eepdata.lighting_flags == 2) {
						// eepdata.lighting_flags = 0;
						// eepdata.lighting_layers = 0;
					// } else eepdata.lighting_layers ? eepdata.lighting_flags++ : eepdata.lighting_layers++; // Drawing a flowchart may help
					// if (eepdata.lighting_flags == 2) {   // if Lighting Layer is enabled for ALL LEDS
						// eepdata.lighting_flags = 0;      // ???
						// eepdata.lighting_layers = false; // LL off
					// } else eepdata.lighting_layers ? eepdata.lighting_flags++ : (eepdata.lighting_layers = false); // Drawing a flowchart may help
				// }
				if (menu_cursor == MENU_KNOBFUNCTION) {
					if (eepdata.knob_func == ENCODER_FUNC_MAX) eepdata.knob_func = 0;
					else eepdata.knob_func++;
				}
			} else {         // Turn counter-clockwise => LEFT
				if (menu_cursor == MENU_ACTIVATELAYER) {
					if (eepdata.active_layer == 0) eepdata.active_layer = DYNAMIC_KEYMAP_LAYER_COUNT-1;
					else eepdata.active_layer--;
				}
				if (menu_cursor == MENU_ANIMATION) {
					if (eepdata.display_anim == 0) eepdata.display_anim = DISPLAY_ANIM_QTY;
					else eepdata.display_anim--;
				}
				if (menu_cursor == MENU_DISPLAYTIMEOUT) {
					eepdata.display_timeout -= DISPLAY_TIMEOUT_STEP;
					if (eepdata.display_timeout <= 0)
						eepdata.display_timeout = DISPLAY_TIMEOUT_NEVER;
				}
				#if defined(QUANTUM_PAINTER_ENABLE)
				if (menu_cursor == MENU_DISPLAYBRIGHTNESS) {
					if (eepdata.display_brightness == 1)
						eepdata.display_brightness = BACKLIGHT_LEVELS;
					else eepdata.display_brightness--;
					backlight_level(eepdata.display_brightness);
				}
				if (menu_cursor == MENU_DISPLAYROTATION) {
					if (eepdata.display_rotation == QP_ROTATION_0)
						eepdata.display_rotation = QP_ROTATION_90; // or QP_ROTATION_270
					else eepdata.display_rotation--;
					display_rotate_flag = true;
					// qp_init(my_display, eepdata.display_rotation);
					// menu_init();
				}
				#endif // defined(QUANTUM_PAINTER_ENABLE)
				// if (menu_cursor == MENU_LIGHTINGLAYERS) {
					// if (eepdata.lighting_layers == 0) {
						// eepdata.lighting_layers = 1;
						// eepdata.lighting_flags = 2;
					// } else eepdata.lighting_flags ? eepdata.lighting_flags-- : (eepdata.lighting_layers = 0);  // Drawing a flowchart may help
				// }
				if (menu_cursor == MENU_KNOBFUNCTION) {
					if (eepdata.knob_func == 0) eepdata.knob_func = ENCODER_FUNC_MAX;
					else eepdata.knob_func--;
				}
			}
		}
		menu_quick_view();
		return false;
	}
	#endif // defined(QUANTUM_PAINTER_ENABLE)
	
	/*** ENCODER'S NORMAL BEHAVIOR ***/
	/* First encoder */
	dprintf("eepdata.knob_func: %u\n", eepdata.knob_func);
    if (index == 0) {
        if (clockwise) { /*** CLOCKWISE ***/
			switch (eepdata.knob_func) {
				case 0:
					break;
				case 1:
					tap_code(ENCODER_FN1_CW);
					break;
				case 2:
					tap_code(ENCODER_FN2_CW);
					break;
				case 3:
					tap_code(ENCODER_FN3_CW);
					break;
				case 4:
					tap_code(ENCODER_FN4_CW);
					break;
				default:
					break;
			}
        } else {        /*** COUNTER CLOCKWISE ***/
			switch (eepdata.knob_func) {
				case 0:
					break;
				case 1:
					tap_code(ENCODER_FN1_CCW);
					break;
				case 2:
					tap_code(ENCODER_FN2_CCW);
					break;
				case 3:
					tap_code(ENCODER_FN3_CCW);
					break;
				case 4:
					tap_code(ENCODER_FN4_CCW);
					break;
				default:
					break;
			}
        }
    }
    return true;
}

bool process_encoder_tap(void) {
	#if defined(QUANTUM_PAINTER_ENABLE)
	if (current_menu == MAIN_MENU) {       // single-press in Menu = execute that line		
		menu_action();
		return true;
	} else if (current_menu == SUB_MENU) { // single-press in Sub-Menu = quit Sub-Menu
		current_menu = MAIN_MENU;
		menu_quick_view();
		/* Special cases handling */
		if (menu_cursor == MENU_ABOUT) { // when exit "About Kawii9", need to re-render Main Menu
			menu_init();
		}
		/*************************/
		return true;
	}
	#endif // defined(QUANTUM_PAINTER_ENABLE)
	
	SEND_STRING(ENCODER_TAP_FN);
	return true;
}
