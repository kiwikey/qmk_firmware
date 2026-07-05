#include QMK_KEYBOARD_H
#include "qp_graphics.h"

#include "display/qp_includes.h"
#include "display/eeprom_custom.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"

painter_device_t my_display;
bool     booting = false; // will be TRUE during boot animation
uint32_t bootup_time = 0;

void display_init(void) {
	my_display = qp_ili9341_make_spi_device(
		ILI9341_WIDTH,
		ILI9341_HEIGHT,
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
	qp_stop_animation(my_anim);

	qp_rect(my_display, 0, 0, 319, 239, HSV_BLACK, true); // Fill screen by black color
	qp_flush(my_display);
	widget_matrix_init();
	widget_layer_init();
	widget_knob_init();
	
	widget_layer_render(0); // temporarily 0, should be get from EEPROM
	widget_matrix_keymap_render(0);

    return 0;   // Don't schedule again
}

void keyboard_post_init_display(void) {
	display_init();
	// booting = true;
	// my_anim = qp_animate(my_display, 0, 90, gif_bootup01);
	// defer_exec(BOOT_DURATION, finish_boot_animation, NULL);

	qp_rect(my_display, 0, 0, 319, 239, HSV_BLACK, true); // Fill screen by black color
	qp_flush(my_display);
	widget_matrix_init();
	widget_layer_init();
	widget_knob_init();
	
	widget_layer_render(0); // temporarily 0, should be get from EEPROM
	widget_matrix_keymap_render(0);
}

bool process_record_display(uint16_t keycode, keyrecord_t *record) {
	if (!booting)
		widget_matrix_update(record->event.key.col, record->event.key.row);
	return true;
}

layer_state_t layer_state_set_kb(layer_state_t state) {
	if (!booting) {
		widget_matrix_bgclear();
		widget_layer_render(get_highest_layer(state));
		widget_matrix_keymap_render(get_highest_layer(state));
	}
	return state;
}
