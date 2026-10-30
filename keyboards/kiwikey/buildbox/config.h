#pragma once

#define FW_VERSION "FW V260927"

#define MATRIX_COLS 4
#define MATRIX_ROWS 5

#define I2C_DRIVER   I2CD1
#define I2C1_SDA_PIN GP10
#define I2C1_SCL_PIN GP11

#define TAP_CODE_DELAY 10

#if defined(COMBO_ENABLE)
	#define COMBO_SHOULD_TRIGGER // enables the combo_should_trigger() override in features/combos.c
#endif // defined(COMBO_ENABLE)

#if defined(VIA_ENABLE)
	#define DYNAMIC_KEYMAP_LAYER_COUNT 4
	#define VIA_EEPROM_CUSTOM_CONFIG_SIZE 26 // must match sizeof(eeprom_custom_t) (features/eeprom_custom.h) - enforced by a _Static_assert in eeprom_custom.c

	// via_extra (layer names, features/via_extra.c): a dedicated region at the very end of EEPROM. VIA's macro
	// buffer normally runs to the last EEPROM byte, so it is ended earlier to leave this tail alone -
	// it costs the macro buffer VIA_EXTRA_EEPROM_SIZE bytes (3871 -> 3711).
	// 4096 = this board's EEPROM size (RP2040 flash emulation); via_extra.c _Static_asserts it.
	#define VIA_EXTRA_EEPROM_SIZE                160
	#define VIA_EXTRA_EEPROM_ADDR                (4096 - VIA_EXTRA_EEPROM_SIZE)
	// Macro names (via_extra) get their own region just below the layer-name region (so the layer names stay where
	// they are), carved out of the macro buffer in the same way: 3709 -> 3529 bytes.
	// 180 = 4 header bytes + 16 records of 11 bytes (length + 10 characters), see via_extra.c.
	#define VIA_EXTRA_MACRO_EEPROM_SIZE    180
	#define VIA_EXTRA_MACRO_EEPROM_ADDR    (VIA_EXTRA_EEPROM_ADDR - VIA_EXTRA_MACRO_EEPROM_SIZE)
	#define DYNAMIC_KEYMAP_EEPROM_MAX_ADDR 3755 // must be a plain number: QMK's config parser can't read an expression. = VIA_EXTRA_MACRO_EEPROM_ADDR - 1, checked by a _Static_assert in via_extra.c
#endif // defined(VIA_ENABLE)

#if defined(QUANTUM_PAINTER_ENABLE)
	/*** SPI DRIVER DEFINITIONS (shared - same SPI bus/DC/RST wiring for both panels) ***/
	#define SPI_DRIVER           SPID1
	#define SPI_SCK_PIN          GP14
	#define SPI_MOSI_PIN         GP15
	#define DISPLAY_DC_PIN       GP12
	#define DISPLAY_RST_PIN      GP9
	#define DISPLAY_SPI_DIVISOR  4
	// #define SPI_SCK_PAL_MODE  5
	// #define SPI_MOSI_PAL_MODE 5
	// #define SPI_MISO_PAL_MODE 5

	/*** TFT DISPLAY CONFIGURATIONS ***/
	#define ILI9341_WIDTH  320
	#define ILI9341_HEIGHT 240
	#define ST7789_WIDTH   320
	#define ST7789_HEIGHT  240

	// 	ILI9341 DRIVER CONFIGS
	#if defined(QUANTUM_PAINTER_ILI9341_SPI_ENABLE)
		#define DISPLAY_CS_PIN   GP8
		#define DISPLAY_SPI_MODE 0
		#define DISPLAY_ROTATION QP_ROTATION_90
		#define DISPLAY_WIDTH    ILI9341_WIDTH
		#define DISPLAY_HEIGHT   ILI9341_HEIGHT
		#define BACKLIGHT_PIN    GP13
	// 	ST7789 DRIVER CONFIGS
	#elif defined(QUANTUM_PAINTER_ST7789_SPI_ENABLE)
		#define DISPLAY_CS_PIN   GP13
		#define DISPLAY_SPI_MODE 3 // ST7789 works with mode 2 or 3 only, not sure why
		#define DISPLAY_ROTATION QP_ROTATION_270
		#define DISPLAY_WIDTH    ST7789_WIDTH
		#define DISPLAY_HEIGHT   ST7789_HEIGHT
		#define BACKLIGHT_PIN    GP8
	#endif

	/*** QUANTUM PAINTER CONFIGURATION ***/
	#define QUANTUM_PAINTER_DISPLAY_TIMEOUT        0   // LCD Timeout handles by custom code
	// #define QUANTUM_PAINTER_SUPPORTS_256_PALETTE   TRUE
	#define QUANTUM_PAINTER_SUPPORTS_NATIVE_COLORS TRUE
	#define QUANTUM_PAINTER_NUM_IMAGES             41   // The maximum number of images/animations that can be loaded at any one time - qp_init_load_files() (qp_includes.c) keeps every image it loads resident (currently 35, +1 briefly for the boot animation), so this must stay >= that count - a shortfall is logged at boot.
	#define QUANTUM_PAINTER_NUM_FONTS	           13 	// The maximum number of fonts that can be loaded at any one time.
	#define QUANTUM_PAINTER_CONCURRENT_ANIMATIONS  2 // The maximum number of animations that can be executed at the same time.
	// #define QUANTUM_PAINTER_DEBUG	               unset  // Prints out significant amounts of debugging information to CONSOLE output. Significant performance degradation, use only for debugging.
	#define QUANTUM_PAINTER_PIXDATA_BUFFER_SIZE	   1024 //The limit of the amount of pixel data that can be transmitted in one transaction to the display. Higher values require more RAM on the MCU.

#endif // defined(QUANTUM_PAINTER_ENABLE)

/*** PWM DRIVER DEFINITIONS ***/
// There is no official API for PWM, so PWM Backlight is used

#if defined(QUANTUM_PAINTER_ILI9341_SPI_ENABLE)
	// GPIO13 - PWM6 B
	#define BACKLIGHT_PWM_DRIVER    PWMD6
	#define BACKLIGHT_PWM_CHANNEL   RP2040_PWM_CHANNEL_B
	#define BACKLIGHT_PAL_MODE      (PAL_MODE_ALTERNATE_PWM | PAL_RP_PAD_DRIVE12 | PAL_RP_GPIO_OE)
	#define BACKLIGHT_DEFAULT_LEVEL 10
#elif defined(QUANTUM_PAINTER_ST7789_SPI_ENABLE)
	// GPIO8 - PWM4 A
	#define BACKLIGHT_PWM_DRIVER    PWMD4
	#define BACKLIGHT_PWM_CHANNEL   RP2040_PWM_CHANNEL_A
	// #define BACKLIGHT_PAL_MODE      2
	#define BACKLIGHT_PAL_MODE      (PAL_MODE_ALTERNATE_PWM | PAL_RP_PAD_DRIVE12 | PAL_RP_GPIO_OE)
	#define BACKLIGHT_DEFAULT_LEVEL 10
#endif
