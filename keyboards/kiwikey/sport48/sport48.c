#include "sport48.h"

#include "color.h"
#include "spi_master.h"

#if defined(QUANTUM_PAINTER_ENABLE)
	#include "qp/qp_includes.h"
	painter_device_t my_display;
#endif // defined(QUANTUM_PAINTER_ENABLE)

void keyboard_post_init_kb(void) {
	#if defined(BACKLIGHT_ENABLE)
		backlight_enable(); // TFT backlight
		backlight_level(10);
	#endif // defined(BACKLIGHT_ENABLE)
	
	#if defined(QUANTUM_PAINTER_ENABLE)
		my_display = qp_st7789_make_spi_device(
			ST7789_WIDTH,
			ST7789_HEIGHT,
			DISPLAY_CS_PIN,
			DISPLAY_DC_PIN,
			DISPLAY_RST_PIN,
			DISPLAY_SPI_DIVISOR,
			DISPLAY_SPI_MODE
		);
		// qp_init(my_display, eepdata.display_rotation);
		qp_init(my_display, QP_ROTATION_90);
		qp_power(my_display, true);
		qp_clear(my_display);
		qp_rect(my_display, 0, 0, 239, 239, HSV_BLACK, true); // Fill screen by black color
		qp_init_load_files();
		qp_drawtext_recolor(my_display, 0, ST7789_HEIGHT-thintel->line_height-5, thintel, " - Sport48 by KiwiKey - ", HSV_WHITE, HSV_BLACK);
	#endif // defined(QUANTUM_PAINTER_ENABLE)
	
	// qp_drawtext(my_display, 0, 0, thintel,        "Thintel-0123456789");
	// qp_drawtext(my_display, 0, 50, roboto20,      "Roboto20-0123456789");
	// qp_drawtext(my_display, 0, 100, roboto25,     "Roboto25-012345");
	// qp_drawtext(my_display, 0, 150, robotobold25, "Robotobold25-01");

    keyboard_post_init_user();
}

void display_render(void) {
	qp_drawtext(my_display, 0, 0,  roboto25, "Layer");
	qp_drawtext(my_display, 0, 30, roboto25, "Layer");
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	return process_record_user(keycode, record);
}
