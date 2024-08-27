#include "sport48.h"

#include "print.h"
#include "spi_master.h"
#include "color.h"

painter_device_t my_display;

void st7789_init(void) {
	// Create my_display
    my_display = qp_st7789_make_spi_device(
		ST7789_WIDTH,
		ST7789_HEIGHT,
		DISPLAY_CS_PIN,
		DISPLAY_DC_PIN,
		DISPLAY_RST_PIN,
		DISPLAY_SPI_DIVISOR,
		DISPLAY_SPI_MODE
	);
    qp_init(my_display, QP_ROTATION_90);   // Initialise the my_display
	qp_power(my_display, true);
	qp_clear(my_display);
}

void keyboard_post_init_kb(void) {
    st7789_init();
	qp_rect(my_display, 0, 0, 239, 239, HSV_BLACK, true);
	
	backlight_enable();
	backlight_level(10);
	
	// Allow for user post-init
    keyboard_post_init_user();
}

// bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	// char buf1[32] = {0};
    // sprintf(buf1, "Layer %d", get_highest_layer(layer_state));
	// qp_drawtext(my_display, 0, 0, arial30, buf1);
	// if (is_keyboard_left()) {
        // SEND_STRING("left process_record_kb");
    // } else {
        // SEND_STRING("right process_record_kb");
    // }
	// char buf1[32] = {0};
    // sprintf(buf1, "Test %d", get_highest_layer(layer_state));
	// qp_drawtext(my_display, 0, 60, arial30, buf1);
	
	// render_solidmatrix_update(record->event.key.col, record->event.key.row);
	
	// return true; // Process all other keycodes normally
// }

//----------------------------------------------------------
// UI Placeholder, implemented in themes

__attribute__((weak)) void render_ui_user(void) {}

void housekeeping_task_kb(void) {
	if (is_keyboard_master()) {
		render_ui_user();
	}
}