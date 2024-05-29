#include "qp.h"
#include "quantum.h"
#include "print.h"
#include "spi_master.h"
#include "color.h"

#include "sport48.h"

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
	qp_rect(my_display, 0, 0, 239, 239, HSV_BLACK, true);
}

void keyboard_post_init_kb(void) {
    st7789_init();
	backlight_level(10);
    keyboard_post_init_user();
}
