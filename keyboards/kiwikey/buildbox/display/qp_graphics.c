#include QMK_KEYBOARD_H

#if defined(QUANTUM_PAINTER_ENABLE)

#include "display/qp_includes.h"
#include "display/eeprom_custom.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"

painter_device_t my_display;

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
		qp_init(my_display, QP_ROTATION_270);   // Initialise my_display
		qp_power(my_display, true);
		qp_clear(my_display);
		qp_rect(my_display, 0, 0, 319, 239, HSV_BLACK, true); // Fill screen by black color
		qp_init_load_files();
	#endif // defined(QUANTUM_PAINTER_ENABLE)
}

bool process_record_display(uint16_t keycode, keyrecord_t *record) {
		widget_matrix_update(record->event.key.col, record->event.key.row);
	return true;
}

layer_state_t layer_state_set_kb(layer_state_t state) {
		widget_layer_render(get_highest_layer(state));
	return state;
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
