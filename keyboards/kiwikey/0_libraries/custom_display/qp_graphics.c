#include QMK_KEYBOARD_H

#if defined(QUANTUM_PAINTER_ENABLE)

#include <printf.h>
#include "qp.h"
#include "qp_graphics.h"
#include "defines.h"
#include "eeprom_custom.h"

#include "qp/qp_custom_api.h"
#include "qp/qp_includes.h"
#include "qp/qp_widget_layer.h"
#include "qp/qp_widget_matrix.h"
#include "qp/qp_widget_rgbstat.h"
#include "qp/qp_menu.h"

bool display_rotate_flag  = false;
bool is_display_on         = false;
// bool rgboff_flag   = false;

void ui_refresh(void) { // TODO: move to 'qp_ui.c'
	is_display_on = true;
	qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, UI_COLOR_BACKGROUND, true); // Clear screen
	
	widget_layer_init();
	widget_layer_render(eepdata.active_layer);
	
	widget_matrix_init();
	
	widget_rgb_init();
	widget_rgb_render();
	
	qp_drawtext_recolor(my_display, 0, ST7789_HEIGHT-thintel->line_height-5, thintel, " - Kiwi5x5 by KiwiKey - ", HSV_WHITE, UI_COLOR_BACKGROUND);
	
	// switch (eepdata.display_anim) {
		// case 0:
			// qp_stop_animation(my_anim);
			// break;
		// case 1:
			// my_anim = qp_animate(my_display, 120, 120, gif_cat01);
			// break;
		// case 2:
			// my_anim = qp_animate(my_display, 120, 120, gif_dog01);
			// break;
		// case 3:
			// my_anim = qp_animate(my_display, 120, 120, gif_nyan120px);
			// break;
		// default:
			// break;
	// }
}

bool display_task_kb(void) {
	// char buf1[50] = {0};
	// sprintf(buf1, "R=%2u  ", eepdata.display_rotation);
	// qp_drawtext(my_display, 0, 0, roboto20, buf1);
	
	/* Manually turn off OLED, if OLED timeout reached & not "Always ON" (even in Menu) */
	if (is_display_on) {
		if ((eepdata.display_timeout != DISPLAY_TIMEOUT_NEVER) && ((last_input_activity_elapsed()/1000) > eepdata.display_timeout)) {
			if (current_menu != NOT_IN_MENU) menu_exit();
			qp_power(my_display, false); // Turn off display
			backlight_level(0);          // Turn off display backlight
			is_display_on = false;
		}
    } else if ((last_input_activity_elapsed()/1000) < eepdata.display_timeout) { // Manually turn on display
		backlight_level(eepdata.display_brightness);
		qp_power(my_display, true); // Turn on display
		ui_refresh();
		is_display_on = true; // not necessary
	}
	
	/* This is used only when rotate the display */
	// TODO: clean up this
	if (display_rotate_flag) {
		qp_init(my_display, eepdata.display_rotation);
		qp_power(my_display, true);
		menu_init();
		qp_drawtext(my_display, 0, ST7789_HEIGHT-roboto20->line_height, roboto20, "<");
		qp_drawtext(my_display, ST7789_WIDTH-roboto20->line_height, ST7789_HEIGHT-roboto20->line_height, roboto20, ">");
		current_menu = SUB_MENU;
		menu_cursor = MENU_DISPLAYROTATION;
		if (eepdata.display_rotation == QP_ROTATION_0)
			swap_hands_off();
		else if (eepdata.display_rotation == QP_ROTATION_90)
			swap_hands_on();
		display_rotate_flag = false;
	}
	
	/*** If in Menu, skip all other DISPLAY processes ***/
    if (current_menu != NOT_IN_MENU) {
		if ((last_input_activity_elapsed()/1000) > DISPLAY_TIMEOUT_MIN) // Turn off Menu automatically after DISPLAY_TIMEOUT_MIN
			menu_exit();
        return false;
    }
	

	
	// Flags check, refresh widget if needed
	// if (qp_widget_layer_flag) {
		// widget_layer_render(eepdata.active_layer);
		// qp_widget_layzer_flag = false;
	// }
	// if (qp_widget_matrix_flag) {
		// widget_matrix_update();
		// qp_widget_matrix_flag = false;
	// }
	if (qp_widget_rgbstat_flag) {
		widget_rgb_render();
		qp_widget_rgbstat_flag = false;
	}
	
	// uint16_t curr_hue = 100;
	// static led_t last_led_state = {0};
	// last_led_state.raw = host_keyboard_led_state().raw;
	// qp_drawimage_recolor(my_display, 12, 120, last_led_state.caps_lock ? lock_caps_on : lock_caps_off, curr_hue, 255, last_led_state.caps_lock ? 255 : 32, curr_hue, 255, 0);
	// qp_drawimage_recolor(my_display, 12+32, 120, last_led_state.num_lock ? lock_num_on : lock_num_off, curr_hue, 255, last_led_state.num_lock ? 255 : 32, curr_hue, 255, 0);
	// qp_drawimage_recolor(my_display, 12+32+32, 120, last_led_state.scroll_lock ? lock_scrl_on : lock_scrl_off, curr_hue, 255, last_led_state.scroll_lock ? 255 : 32, curr_hue, 255, 0);
	return false;
}

bool process_record_display(uint16_t keycode, keyrecord_t *record) {
	if (current_menu != NOT_IN_MENU) { // If being in MENU, only process the encoder, skip all other keycodes
		if (record->event.key.col != 5) // If not encoder press, leave here (only 1 condition is needed)
			return false;
    }
	
	#if defined(QP_WIDGET_MATRIX)
		if (record->event.key.col != 5) // TODO: hotfix, do not render "key matrix" if the encoder is pressed
			widget_matrix_update(record->event.key.col, record->event.key.row);
	#endif // defined(QP_WIDGET_MATRIX)
	
	// if (is_display_on) {
		// qp_power(my_display, true); // Turn on display
		// backlight_level(eepdata.display_brightness); // Turn on display backlight
	// }
	
	switch (keycode) {
        case QK_LIGHTING ... QK_LIGHTING_MAX:
			qp_widget_rgbstat_flag = true;
            break;
        // case CUSTOM_KC_MENU:
            // if (current_menu == NOT_IN_MENU) {
				// if (!rgb_matrix_is_enabled()) {
					// rgb_matrix_enable_noeeprom();
					// rgboff_flag = true;
				// }
				// qp_stop_animation(my_anim);
				// current_menu = MAIN_MENU;
				// layer_move(eepdata.active_layer); // to avoid weird behavior of current_layer when turn on MENU
				// menu_init();
			// }
			// return false; // no need to process this keycode
            // break;
        // case CUSTOM_KC_REFRESH:
			// if (record->event.pressed) {
				// qp_stop_animation(my_anim);
				// ui_refresh();
			// }
			// //return false; // no need to process this keycode
            // break;
        default:
            break;
    }
	return true;
}

layer_state_t layer_state_set_kb(layer_state_t state) {
	if (current_menu == NOT_IN_MENU)
		widget_layer_render(get_highest_layer(state));
	return state;
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
