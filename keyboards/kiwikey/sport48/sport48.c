#include "sport48.h"

#include "color.h"
#include "spi_master.h"
#include "raw_hid.h"

#include "eeprom_custom.h"

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
		qp_power(my_display, true);
		qp_clear(my_display);
		qp_rect(my_display, 0, 0, 239, 239, HSV_BLACK, true); // Fill screen by black color
		qp_init_load_files();
		// ui_refresh();
		qp_drawtext_recolor(my_display, 0, ST7789_HEIGHT-thintel->line_height-5, thintel, " - Sport48 by KiwiKey - ", HSV_WHITE, UI_COLOR_BACKGROUND);
	#endif // defined(QUANTUM_PAINTER_ENABLE)
	
	print("- Init done! \n");
	
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


void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
// void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
	// Create data array, size 64 bytes, and fill with 0x00 (to avoid weird data)
	
	// uint8_t dataHIDhandshake[64];
	// uint8_t dataHIDlayout[64];
	// for (int i = 0; i < 64; i++) {
		// dataHIDhandshake[i] = 0x00;
		// dataHIDlayout[i] = 0x00;
	// }

	char buf1[32] = {0};
	for (uint8_t i = 0; i<12; i++) {
		sprintf(buf1, "%d", data[i]);
		qp_drawtext(my_display, 10+i*16, 0, thintel, buf1);
	}
	for (uint8_t i = 0; i<12; i++) {
		sprintf(buf1, "%d", data[i+12]);
		qp_drawtext(my_display, 10+i*16, 20, thintel, buf1);
	}
	for (uint8_t i = 0; i<12; i++) {
		sprintf(buf1, "%d", data[i+12+12]);
		qp_drawtext(my_display, 10+i*16, 40, thintel, buf1);
	}
	
	for (uint8_t i = 0; i<32; i++) {
		uprintf("%d-", data[i]);
	}
	printf("\n");
	
	uint8_t dataHIDhandshake[32] = {0xaa,0xcc,0x55,'z','y'};
	raw_hid_send(dataHIDhandshake, length);
	
	// Define response package
	// dataHIDhandshake[0] = 0xAA;
	// for (int i = 1; i <= 9; i++) {
		// dataHIDlayout[i] = i+5;
	// }
	
	// if (data[0] == 0x55) {
		// raw_hid_send(dataHIDhandshake, length);		// If receive 0x55, confirm handshake
		// rgblight_step();
	// }
	// if (data[0] == 0x69) {
		// raw_hid_send(dataHIDlayout, length);		// If receive 0x69, send layout
		// rgblight_step();
	// }
	// else {
		// raw_hid_send(data, length);					// Or else, just echo what received
	// }
}