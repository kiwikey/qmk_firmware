#include "buildbox.h"

#include <qp.h>
// #include <lib/lib8tion/lib8tion.h>
#include "print.h"

#include "magnetic_encoder.h"

#if defined(QUANTUM_PAINTER_ENABLE)
	#include "display/qp_graphics.h"
	#include "display/qp_includes.h"
	#include "display/qp_custom_api.h"
	#include "display/eeprom_custom.h"
	#include "display/widgets/qp_widget_matrix.h"
	#include "display/widgets/qp_widget_layer.h"
	#include "display/widgets/qp_widget_knob.h"
#endif // defined(QUANTUM_PAINTER_ENABLE)

void keyboard_post_init_kb(void) {
	keyboard_post_init_display();
	keyboard_post_init_user();
}

void housekeeping_task_kb(void) {
	// widget_matrix_keymap_render();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	process_record_display(keycode, record);
	return process_record_user(keycode, record);
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
