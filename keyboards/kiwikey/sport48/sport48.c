#include "sport48.h"
#include "eeprom_custom.h"
#include "color.h"
#include "spi_master.h"

#if defined(QUANTUM_PAINTER_ENABLE)
    #include "qp_graphics.h"	
	#include "qp/qp_includes.h"
	#include "qp/qp_menu.h"
	painter_device_t my_display;
#endif // defined(QUANTUM_PAINTER_ENABLE)

#if defined(CONSOLE_ENABLE)
    #include "print.h"
#endif // defined(CONSOLE_ENABLE)

EEPROM_CUSTOM_DATA eepdata;
EEPROM_CUSTOM_DATA eepdata_default = {
	0,                                // active_layer       : 0
	1,                                // display_anim       : QMK Logo
	30,                               // display_timeout    : 30s
	0,                                // display_brightness :
	0,                                // display_rotation   : (OLED_ROTATION_0 = 0)
	0,                                // lighting_layers    : OFF
	0,                                // lighting_flags     : applied to Underglow LEDs
	{ 126, 210,  42,  84 }, // layer_hue          : Cyan - Magenta - Yellow - Green - Blue - null
	{ 255, 255, 255, 255 }, // layer_sat          : maximum (255)
	1,                                // knob_func          : Volume
	7                                 // checksum           : 7 (always)
};

void keyboard_post_init_kb(void) {
	#if defined(CONSOLE_ENABLE)
		debug_enable   = true; // everything
		debug_matrix   = false; // col/row datas
		debug_keyboard = false; // ???
		debug_mouse    = false; // ???
    #endif // defined(CONSOLE_ENABLE)
	
	#if defined(BACKLIGHT_ENABLE)
		backlight_enable(); // TFT backlight
		// backlight_level(eepdata.display_brightness);
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
		ui_refresh();
	#endif // defined(QUANTUM_PAINTER_ENABLE)
	
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