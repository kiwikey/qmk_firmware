#include "kiwi5x5.h"

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
    qp_init(my_display, QP_ROTATION_0);   // Initialise the my_display
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

//----------------------------------------------------------
// UI Placeholder, implemented in themes

__attribute__((weak)) void render_ui_user(void) {}

void housekeeping_task_kb(void) {
	// if (is_keyboard_master()) {
		render_ui_user();
	// }
}
