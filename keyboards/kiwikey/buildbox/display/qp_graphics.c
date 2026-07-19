#include QMK_KEYBOARD_H
#include "qp_graphics.h"

#include "features/eeprom_custom.h"
#include "sensor/sensors_handler.h"
#include "display/qp_includes.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"
#include "display/widgets/qp_menu.h"
#include "display/widgets/qp_widget_breakout.h"

painter_device_t my_display;
bool     booting = false; // will be TRUE during boot animation

uint16_t flag_display_keycode_changed = 0x0000;
// flag_display_keycode_changed: contains layer, row, col of changed key
// Mask:     00          00      00       00
//       is changed?   layer     row      col
// Example: 0x1231 = changed, layer 2, row 3, col 1

uint8_t flag_widget_layer_changed = 0;
// 0 = nothing changed (we need this, so other layers need to +1)
// 1 = layer 0 changed
// 2 = layer 1 changed

void display_init(void) {
	// my_display = qp_ili9341_make_spi_device(
	// 	ILI9341_WIDTH,
	// 	ILI9341_HEIGHT,
	// 	DISPLAY_CS_PIN,
	// 	DISPLAY_DC_PIN,
	// 	DISPLAY_RST_PIN,
	// 	DISPLAY_SPI_DIVISOR,
	// 	DISPLAY_SPI_MODE
	// );
	my_display = qp_st7789_make_spi_device(
		ST7789_WIDTH,
		ST7789_HEIGHT,
		DISPLAY_CS_PIN,
		DISPLAY_DC_PIN,
		DISPLAY_RST_PIN,
		DISPLAY_SPI_DIVISOR,
		DISPLAY_SPI_MODE
	);
	qp_init(my_display, QP_ROTATION_270);   // Initialise my_display
	qp_power(my_display, true);
	qp_clear(my_display);
	qp_rect(my_display, 0, 0, 319, 239, HSV_BLACK, true); // Fill screen by black color
	qp_flush(my_display);
	qp_init_load_files();
}

uint32_t finish_boot_animation(uint32_t trigger_time, void *cb_arg) {
    booting = false;
	accumulator = 0; // All knob rotation during boot animation is cleared
	qp_stop_animation(my_anim);
	ui_refresh();
    return 0;   // Don't schedule again
}

void keyboard_post_init_display(void) {
	display_init();
	if (eepdata.display_bootanim == 1) {
		booting = true;
		my_anim = qp_animate(my_display, 0, 90, gif_bootup01);
		defer_exec(BOOT_DURATION, finish_boot_animation, NULL);
	} else {
		ui_refresh();
	}
}

void ui_refresh(void) {
	qp_rect(my_display, 0, 0, 319, 239, HSV_BLACK, true); // Fill screen by black color
	qp_flush(my_display);
	widget_matrix_init();
	widget_layer_init();
	widget_knob_init();
	widget_layer_render(get_highest_layer(layer_state));
	widget_matrix_keymap_render(get_highest_layer(layer_state));
	qp_flush(my_display);
}

void housekeeping_task_display(void) { // Check all flags
	if (flag_display_keycode_changed & 0x1000) {
		uint16_t layer = (flag_display_keycode_changed & 0x0F00) >> 8;
		if (layer == get_highest_layer(layer_state)) { // only process if that changed layer is being activated
			uint16_t row = (flag_display_keycode_changed & 0x00F0) >> 4;
			uint16_t col = flag_display_keycode_changed & 0x00F;
			widget_matrix_render_singlebutton(row,
											  col,
											  WIDGET_MATRIX_BUTTON_OFF,
											  true,
											  layer);
		}
		flag_display_keycode_changed = 0x0000;
	}

	if (flag_widget_layer_changed) { // 0 means nothing changed
		if ((flag_widget_layer_changed - 1) == get_highest_layer(layer_state)) {
			widget_layer_number_render(flag_widget_layer_changed - 1);
			widget_layer_render_layername(flag_widget_layer_changed - 1);
		}
		flag_widget_layer_changed = 0;
	}
}

bool process_record_display(uint16_t keycode, keyrecord_t *record) {
	if (booting) return false;

	/*** If playing Breakout :
		+ Pressing Button 1 -> exit the game (back to default screen)
		+ Pressing Button 2 -> launch the ball / restart after game over or win
	***/
	if (breakout_is_active()) {
		if (record->event.pressed) {
			switch (keycode) {
				case KC_BUTTON_1:
					breakout_exit();
					return false;
				case KC_BUTTON_2:
					breakout_button_action();
					return false;
				default:
					return false; // During the game, no keycode is processed
			}
		} else return false;
	}

	/*** If being in MENU :
	MAIN MENU :
		+ Pressing Button 1 -> quit Main Menu (back to default screen)
		+ Pressing Button 2 -> menu_action(): run cursor_pos function
	SUB MENU :
		+ Pressing Button 1 -> quit Sub Menu without saving
		+ Pressing Button 2 -> quit Sub Menu and save the setting
	***/
	if (menu_state == MAIN_MENU) {
		if (record->event.pressed) {
			switch (keycode) {
				case KC_BUTTON_1:
					menu_exit();
					return false;
				case KC_BUTTON_2:
					menu_action();
					return false;
				default:
					return false; // During Menu, no keycode is processed
			}
		} else return false;
	} else if (menu_state == SUB_MENU) {
		if (record->event.pressed) {
			switch (keycode) {
				case KC_BUTTON_1:
					menu_submenu_exit();
					return false;
				case KC_BUTTON_2:
					menu_submenu_exit();
					return false;
				default:
					return false; // During Menu, no keycode is processed
			}
		} else return false;
	}

	/*** If not in MENU ***/
	if (record->event.pressed) {
		switch (keycode) {
			case KC_BUTTON_1:
				if (get_highest_layer(layer_state) >= DYNAMIC_KEYMAP_LAYER_COUNT-1)
					layer_move(0);
				else
					layer_move(get_highest_layer(layer_state)+1);
				return false;
			case KC_BUTTON_2:
					if (menu_state == NOT_IN_MENU) {
						menu_init();
					}
					else if (menu_state == MAIN_MENU) {
						menu_exit();
					}
				return false;
			default:
				break; // Process all other keycodes normally
		}
	}

	widget_matrix_update(record->event.key.col, record->event.key.row);
	return true;
}
