#pragma once

#define FW_VERSION "FW V260904"

#define MATRIX_COLS 4
#define MATRIX_ROWS 5

#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP10
#define I2C1_SCL_PIN GP11

#define TAP_CODE_DELAY 10

#if defined(COMBO_ENABLE)
	#define COMBO_SHOULD_TRIGGER // enables the combo_should_trigger() override in features/combos.c
#endif // defined(COMBO_ENABLE)

#if defined(VIA_ENABLE)
	#define DYNAMIC_KEYMAP_LAYER_COUNT 4
	#define VIA_EEPROM_CUSTOM_CONFIG_SIZE 19 // must match sizeof(EEPROM_CUSTOM_DATA) - bump this whenever a field is added/removed
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
	#define QUANTUM_PAINTER_SUPPORTS_256_PALETTE   TRUE
	#define QUANTUM_PAINTER_SUPPORTS_NATIVE_COLORS TRUE
	#define QUANTUM_PAINTER_NUM_IMAGES             16   // The maximum number of images/animations that can be loaded at any one time.
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
