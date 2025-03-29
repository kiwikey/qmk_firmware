#include "kiwi5x5.h"

#include "color.h"
#include "spi_master.h"

#include "eeprom_custom.h"
#include "encoder.h"

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
	0,                           // Layer 0
	1,                           // Animation #1
	DISPLAY_TIMEOUT_MIN,         // LCD Timeout 30s
	BACKLIGHT_DEFAULT_LEVEL,     // LCD Brightness default (10 = max)
	QP_ROTATION_0,               // Default rotation
	0,                           // Lighting Layers OFF
	0,                           // Lighting Layers applied to Underglow LEDs
	{ 126, 210,  42,  84 },      // Lighting Layers' HUEs: Cyan - Magenta - Yellow - Green
	{ 255, 255, 255, 255 },      // Lighting Layers' SATs: maximum (255)
	1,                           // Knob: Volume
	7                            // Checksum is always 7
};

void keyboard_post_init_kb(void) {
    #if defined(CONSOLE_ENABLE)
		debug_enable   = true; // everything
		debug_matrix   = false; // col/row datas
		debug_keyboard = false; // ???
		debug_mouse    = false; // ???
    #endif // defined(CONSOLE_ENABLE)
  
	// Reading all EEPROM custom datas, refer to 'eeprom_custom.h' for detail
    eeprom_read_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));

	/*** Validation check ***/
	/* This runs everytime the EEPROM is corrupted, or right after 'factory_reset' or 'bootmagic_reset' */
	if (eepdata.checksum != 7) {
		eeprom_update_block(&eepdata_default, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
		// Reading all EEPROM custom datas, again
		eeprom_read_block(&eepdata, ((void*)(VIA_EEPROM_CUSTOM_CONFIG_ADDR)), sizeof(EEPROM_CUSTOM_DATA));
	}
	
	// if (eepdata.display_timeout <= 0)
		// eepdata.display_timeout = DISPLAY_TIMEOUT_MIN; // TODO: this is just a hotfix, need to dig in
	// if ((eepdata.display_anim > DISPLAY_ANIM_QTY) || (eepdata.display_anim < 0))
		// eepdata.display_anim = 1;
	// if ((eepdata.display_rotation > QP_ROTATION_270) || (eepdata.display_rotation < QP_ROTATION_0))
		// eepdata.display_rotation = QP_ROTATION_0;
	/************************/
	
	layer_move(eepdata.active_layer);
	
	// Kiwi5x5 ONLY: swap_hand relates to keyboard's rotation and also QP_ROTATION
	#if defined(SWAP_HANDS_ENABLE)
	if (eepdata.display_rotation == QP_ROTATION_0)
		swap_hands_off();
	else if (eepdata.display_rotation == QP_ROTATION_90)
		swap_hands_on();
	#endif // defined(SWAP_HANDS_ENABLE)
		
	#if defined(BACKLIGHT_ENABLE)
		backlight_enable(); // TFT backlight
		backlight_level(eepdata.display_brightness);
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
		qp_init(my_display, eepdata.display_rotation);
		qp_power(my_display, true);
		qp_clear(my_display);
		qp_rect(my_display, 0, 0, 239, 239, HSV_BLACK, true); // Fill screen by black color
		qp_init_load_files();
		ui_refresh();
	#endif // defined(QUANTUM_PAINTER_ENABLE)

    keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	#if defined(QUANTUM_PAINTER_ENABLE)
        if (process_record_display(keycode, record) == false) // process_record_display = false if being in Menu
            return false;
	#endif // defined(QUANTUM_PAINTER_ENABLE)
	
	return process_record_user(keycode, record);
	// uprintf("get_matrix_scan_rate: %lu\n", get_matrix_scan_rate());
}

bool encoder_update_kb(uint8_t index, bool clockwise) { // Rotating
    if (!encoder_update_user(index, clockwise)) {
        return false; /* Don't process further events if user function exists and returns false */
    }
    process_encoder_rotate(index, clockwise);
    return true;
}

#if defined(QUANTUM_PAINTER_ENABLE)
//----------------------------------------------------------
// UI Placeholder, implemented in themes

// __attribute__((weak)) bool display_task_kb(void) {}

void housekeeping_task_kb(void) {
	display_task_kb(); // works as oled_task_kb(), but for QP
}
#endif // defined(QUANTUM_PAINTER_ENABLE)

// Kiwi5x5 ONLY: Overrides default bootmagic
void bootmagic_scan(void) {
    wait_ms(100);
    matrix_scan();
    wait_ms(100); // wait_ms(BOOTMAGIC_DEBOUNCE);
    matrix_scan();
    if ((matrix_get_row(4) & (1 << 5)) || (matrix_get_row(BOOTMAGIC_ROW) & (1 << BOOTMAGIC_COLUMN))) {
		eeconfig_disable();	
		bootloader_jump();
    }
}
