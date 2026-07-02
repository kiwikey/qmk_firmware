#include "buildbox.h"

#include <qp.h>
#include <lib/lib8tion/lib8tion.h>
#include "print.h"
#include "color.h"

#include "spi_master.h"
#include "i2c_master.h"
#include "magnetic_encoder.h"

#include "nvm_eeprom_via_internal.h"
#include "nvm_eeprom_eeconfig_internal.h"

#if defined(QUANTUM_PAINTER_ENABLE)
	#include "display/qp_graphics.h"
	#include "display/qp_includes.h"
	#include "display/qp_custom_api.h"
	#include "display/eeprom_custom.h"
	#include "display/widgets/qp_widget_matrix.h"
	#include "display/widgets/qp_widget_layer.h"
	#include "display/widgets/qp_widget_knob.h"
	painter_device_t my_display;
#endif // defined(QUANTUM_PAINTER_ENABLE)

EEPROM_CUSTOM_DATA eepdata;

void display_init(void) {	
	#if defined(QUANTUM_PAINTER_ENABLE)
		my_display = qp_ili9341_make_spi_device(
			ILI9341_WIDTH,
			ILI9341_HEIGHT,
			DISPLAY_CS_PIN,
			DISPLAY_DC_PIN,
			DISPLAY_RST_PIN,
			DISPLAY_SPI_DIVISOR,
			DISPLAY_SPI_MODE
		);
		qp_init(my_display, QP_ROTATION_270);   // Initialise the my_display
		qp_power(my_display, true);
		qp_clear(my_display);
		qp_rect(my_display, 0, 0, 319, 239, HSV_BLACK, true); // Fill screen by black color
		qp_init_load_files();
	#endif // defined(QUANTUM_PAINTER_ENABLE)
}

void keyboard_post_init_kb(void) {
	// debug_enable=true;
	// debug_matrix=true;
	// debug_keyboard=true;
	// debug_mouse=true;
	
    display_init();
	widget_matrix_init();
	widget_layer_init();
	widget_knob_init();
	
	widget_layer_render(0);

	keyboard_post_init_user();
}

void housekeeping_task_kb(void) {
	widget_matrix_keymap_render();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	process_record_display(keycode, record);
	return process_record_user(keycode, record);
}

bool process_record_display(uint16_t keycode, keyrecord_t *record) {
	#if defined(QP_WIDGET_MATRIX)
		widget_matrix_update(record->event.key.col, record->event.key.row);
	#endif // defined(QP_WIDGET_MATRIX)
	return true;
}

void magnetic_encoder_update_user(magnetic_encoder_kind_t magnetic_encoder, bool direction) {
	// int16_t angle = as5600read_angle();
	// int16_t angle100 = angle/41;
	// int16_t angle360 = angle/(4096/360);
	
	// printf("+ direction = %s \n", direction? "DOWN" : "UP");
	// printf("+ get_distance = %u \n", get_distance(magnetic_encoder));
	// printf("+ get_movement = %u \n", get_movement(100, magnetic_encoder));

	printf("--- get_keycode_string --- \n");
	for (uint8_t i = 0;  i < MATRIX_ROWS; i++) {
		for (uint8_t j = 0;  j < MATRIX_COLS; j++) {
			printf("%10s", get_keycode_string(dynamic_keymap_get_keycode(0, i, j)));
		}
		printf("\n");
	}
	printf("--- keycode_to_string --- \n");
	for (uint8_t i = 0;  i < MATRIX_ROWS; i++) {
		for (uint8_t j = 0;  j < MATRIX_COLS; j++) {
			printf("%10s", keycode_to_string(dynamic_keymap_get_keycode(0, i, j)));
		}
		printf("\n");
	}
	printf("--- keycode --- \n");
	for (uint8_t i = 0;  i < MATRIX_ROWS; i++) {
		for (uint8_t j = 0;  j < MATRIX_COLS; j++) {
			printf("%10x", dynamic_keymap_get_keycode(0, i, j));
		}
		printf("\n");
	}
	printf("\n");
}
