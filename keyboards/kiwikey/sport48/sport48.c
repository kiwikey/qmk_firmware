#include "sport48.h"

#include "color.h"
#include "spi_master.h"
#include "raw_hid.h"
#include "via.h"

#include "eeprom_custom.h"

#include "graphics/lvgl.h"
#include "graphics/display.h"

#if defined(QUANTUM_PAINTER_ENABLE)
    #include "qp_graphics.h"	
	#include "qp/qp_includes.h"
	#include "qp/qp_menu.h"
	painter_device_t my_display;
	bool qp_lvgl_attach(painter_device_t device);
#endif // defined(QUANTUM_PAINTER_ENABLE)

#if defined(CONSOLE_ENABLE)
    #include "print.h"
#endif // defined(CONSOLE_ENABLE)

void init_render(void);

EEPROM_CUSTOM_DATA eepdata;
EEPROM_CUSTOM_DATA eepdata_default = {
	0,                                // active_layer       : 0
	1,                                // display_anim       : QMK Logo
	30,                               // display_timeout    : 30s
	0,                                // display_brightness :
	0,                                // display_rotation   : (QP_ROTATION_0 = 0)
	0,                                // lighting_layers    : OFF
	0,                                // lighting_flags     : applied to Underglow LEDs
	{ 126, 210,  42,  84 },           // layer_hue          : Cyan - Magenta - Yellow - Green
	{ 255, 255, 255, 255 },           // layer_sat          : maximum (255)
	1,                                // knob_func          : Volume
	7                                 // checksum           : 7 (always)
};

void keyboard_post_init_kb(void) {
	#if defined(CONSOLE_ENABLE)
		debug_enable   = false; // everything
		debug_matrix   = false; // col/row datas
		debug_keyboard = false; // ???
		debug_mouse    = false; // ???
    #endif // defined(CONSOLE_ENABLE)
	
	#if defined(VIA_ENABLE)
		// Reading all EEPROM custom datas, refer to 'eeprom_custom.h' for detail
		eeprom_read_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));

		/*** Validation check ***/
		/* This runs everytime the EEPROM is corrupted, or right after 'factory_reset' or 'bootmagic_reset' */
		if (eepdata.checksum != 7) {
			eeprom_update_block(&eepdata_default, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
			// Reading all EEPROM custom datas, again
			eeprom_read_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
		}
		
		layer_move(eepdata.active_layer);
	#endif // defined(VIA_ENABLE)
	
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
		// qp_set_viewport_offsets(my_display, 50, 50);
		qp_power(my_display, true);
		qp_clear(my_display);
		qp_rect(my_display, 0, 0, 239, 239, HSV_BLACK, true); // Fill screen by black color
		qp_init_load_files();
		// ui_refresh();
		qp_drawtext_recolor(my_display, 0, ST7789_HEIGHT-thintel->line_height-5, thintel, " - Sport48 by KiwiKey - ", HSV_WHITE, UI_COLOR_BACKGROUND);
	#endif // defined(QUANTUM_PAINTER_ENABLE)
	
	init_render();
	
    keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	// #if defined(QUANTUM_PAINTER_ENABLE)
        // if (process_record_display(keycode, record) == false) // process_record_display = false if being in Menu
            // return false;
	// #endif // defined(QUANTUM_PAINTER_ENABLE)
	return process_record_user(keycode, record);
}

#if defined(QUANTUM_PAINTER_ENABLE)
//----------------------------------------------------------
// UI Placeholder, implemented in themes
// __attribute__((weak)) bool display_task_kb(void) {}
void housekeeping_task_kb(void) {
	// if (is_keyboard_master()) {
		// display_task_kb(); // works as oled_task_kb(), but for QP
	// }
}
#endif // defined(QUANTUM_PAINTER_ENABLE)

void init_render(void) {
	char buf1[24] = {0};
	qp_drawtext(my_display, 0, 0, thintel, "0123456789123456789");
	sprintf(buf1,"QP:     %s",QUANTUM_PAINTER_ENABLE?"ON":"OFF");
	qp_drawtext(my_display, 0, 20, thintel, buf1);
	sprintf(buf1,"Dim:    %dx%d",qp_get_width(my_display),qp_get_height(my_display));
	qp_drawtext(my_display, 0, 40, thintel, buf1);
	sprintf(buf1,"Rot:    %d", qp_get_rotation(my_display)*90);
	qp_drawtext(my_display, 0, 60, thintel, buf1);
	sprintf(buf1,"Offset: %d,%d", qp_get_offset_x(my_display), qp_get_offset_y(my_display));
	qp_drawtext(my_display, 0, 80, thintel, buf1);
	
	print("- Init done! \n");
}

bool gfx_task_kb(void) {
    static bool initialized = false;
    if (!initialized) {
        lv_obj_t *label = lv_label_create(lv_scr_act());
        lv_label_set_text(label, "Hello World!");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        initialized = true;
    }

    return false; // Let QMK call gfx_task_user next
}