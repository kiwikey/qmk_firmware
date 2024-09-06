#include QMK_KEYBOARD_H

#if defined(QUANTUM_PAINTER_ENABLE)

#include <printf.h>
#include "qp.h"
#include "qp_graphics.h"
#include "qp_custom_api.h"
#include "qp_includes.h"
#include "defines.h"

#include "qp_widget_layer.h"
#include "qp_widget_matrix.h"
#include "qp_widget_rgbstat.h"
#include "qp_menu.h"

EEPROM_CUSTOM_DATA eepdata;

uint8_t refresh_flag = 0;
bool    lcdoff_flag = false;
bool    rgboff_flag = false;

void ui_refresh(void) {
	// qp_drawimage(my_display, 0, 0, img_pikachu);
	qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, UI_COLOR_BACKGROUND, true); // Clear screen
	
	widget_layer_init();
	widget_layer_render(eepdata.active_layer);
	
	widget_matrix_init();
	
	widget_rgb_init();
	widget_rgb_render();
	
	qp_drawtext_recolor(my_display, 0, ST7789_HEIGHT-thintel->line_height, thintel, "Kiwi5x5 by KiwiKey", HSV_WHITE, UI_COLOR_BACKGROUND);
	
	switch (eepdata.oled_anim) {
		case 0:
			qp_stop_animation(my_anim);
			break;
		case 1:
			my_anim = qp_animate(my_display, 120, 120, gif_cat01);
			break;
		case 2:
			my_anim = qp_animate(my_display, 120, 120, gif_dog01);
			break;
		case 3:
			my_anim = qp_animate(my_display, 120, 120, gif_nyan120px);
			break;
		default:
			break;
	}
}

void keyboard_post_init_display(void) {
	qp_init_load_files();

	// Reading all EEPROM custom data
	eeprom_read_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
	
	/*** Validation check ***/
	if (eepdata.oled_timeout <= 0) eepdata.oled_timeout = 30; // TODO: this is just a hotfix, need to dig in
	if ((eepdata.oled_anim > QP_ANIM_QTY) || (eepdata.oled_anim < 0)) eepdata.oled_anim = 1;
	/************************/
	
	layer_move(eepdata.active_layer);
	
#if defined(BACKLIGHT_ENABLE)
	backlight_enable(); // TFT backlight
	backlight_level(eepdata.oled_brightness);
#endif // defined(BACKLIGHT_ENABLE)

	ui_refresh();
}

void housekeeping_task_display(void) {
	if ((eepdata.oled_timeout != QP_TIMEOUT_NEVER) && (last_matrix_activity_elapsed() > eepdata.oled_timeout*1000)) {
		qp_power(my_display, false); // Turn off display
		backlight_level(0);          // Turn off display backlight
		lcdoff_flag = 1;
    }
	
	if (refresh_flag == 1) {
		ui_refresh();		
		refresh_flag = 0;
	}
	
	// Flags check, refresh widget if needed
	if (qp_widget_layer_flag) {
		widget_layer_render(eepdata.active_layer);
		qp_widget_layer_flag = false;
	}
	// if (qp_widget_matrix_flag) {
		// widget_matrix_update();
		// qp_widget_matrix_flag = false;
	// }
	if (qp_widget_rgbstat_flag) {
		widget_rgb_render();
		qp_widget_rgbstat_flag = false;
	}
	
	char buf1[50] = {0};
	sprintf(buf1, "RGB: %u", rgb_matrix_is_enabled() ? 1 : 0);
	qp_drawtext(my_display, 40, ST7789_HEIGHT-roboto20->line_height*2, roboto20, buf1);
	
	// uint16_t curr_hue = 100;
	// static led_t last_led_state = {0};
	// last_led_state.raw = host_keyboard_led_state().raw;
	// qp_drawimage_recolor(my_display, 12, 120, last_led_state.caps_lock ? lock_caps_on : lock_caps_off, curr_hue, 255, last_led_state.caps_lock ? 255 : 32, curr_hue, 255, 0);
	// qp_drawimage_recolor(my_display, 12+32, 120, last_led_state.num_lock ? lock_num_on : lock_num_off, curr_hue, 255, last_led_state.num_lock ? 255 : 32, curr_hue, 255, 0);
	// qp_drawimage_recolor(my_display, 12+32+32, 120, last_led_state.scroll_lock ? lock_scrl_on : lock_scrl_off, curr_hue, 255, last_led_state.scroll_lock ? 255 : 32, curr_hue, 255, 0);
}

bool process_record_display(uint16_t keycode, keyrecord_t *record) {
	// if (!record->event.pressed) { // reconsidering...
		// return true;
	// }
	if (lcdoff_flag) {
		qp_power(my_display, true); // Turn on display
		backlight_level(eepdata.oled_brightness); // Turn on display backlight
	}
    if (current_menu != NOT_IN_MENU) { // in MENU all keys are for controlling, no keycode is sent
		process_record_menu(keycode, record);
		return false;
    }
	widget_matrix_update(record->event.key.col, record->event.key.row);
	
	switch (keycode) {
        case QK_LIGHTING ... QK_LIGHTING_MAX:
            // sub_ui_mode = 1;
            // sub_ui_clear();
			qp_widget_rgbstat_flag = true;
            break;
        case CUSTOM_KC_MENU:
            if (current_menu == NOT_IN_MENU) {
// #if defined(???)
				if (!rgb_matrix_is_enabled()) {
					rgb_matrix_enable_noeeprom();
					rgboff_flag = true;
				}
// #endif // defined(???)
				qp_stop_animation(my_anim);
				current_menu = MAIN_MENU;
				layer_move(eepdata.active_layer); // to avoid weird behavior of current_layer when turn on MENU
				menu_init();
			}
			return false; // no need to process this keycode
            break;
        case CUSTOM_KC_REFRESH:
			if (record->event.pressed) ui_refresh();
			return false; // no need to process this keycode
            break;
        default:
            break;
    }
	return true;
}

layer_state_t layer_state_set_display(layer_state_t state) {
	if (current_menu == NOT_IN_MENU)
		widget_layer_render(get_highest_layer(state));
	return state;
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
