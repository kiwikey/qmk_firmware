#pragma once

#define FW_VERSION "260715"

#define MATRIX_COLS 4
#define MATRIX_ROWS 5

#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP10
#define I2C1_SCL_PIN GP11

#define TAP_CODE_DELAY 10

#if defined(VIA_ENABLE)
	#define DYNAMIC_KEYMAP_LAYER_COUNT 4
	#define VIA_EEPROM_CUSTOM_CONFIG_SIZE 17
#endif // defined(VIA_ENABLE)

#if defined(QUANTUM_PAINTER_ENABLE)
	/*** SPI DRIVER DEFINITIONS ***/
	#define SPI_DRIVER SPID1
	#define SPI_SCK_PIN          GP14
	#define SPI_MOSI_PIN         GP15

	// #define SPI_SCK_PAL_MODE  5
	// #define SPI_MOSI_PAL_MODE 5
	// #define SPI_MISO_PAL_MODE 5

	#define DISPLAY_CS_PIN       GP13
	#define DISPLAY_DC_PIN       GP12
	#define DISPLAY_RST_PIN      GP9

	#define DISPLAY_SPI_DIVISOR 4
	#define DISPLAY_SPI_MODE    3 // ST7789 240*240 works with mode 2 or 3 only, not sure why
	
	/*** TFT DISPLAY CONFIGURATIONS ***/
	#define ST7789_WIDTH  320
	#define ST7789_HEIGHT 240
	
	#define ILI9341_WIDTH  320
	#define ILI9341_HEIGHT 240

	/*** QUANTUM PAINTER CONFIGURATION ***/
	#define QUANTUM_PAINTER_DISPLAY_TIMEOUT 60000
	#define QUANTUM_PAINTER_SUPPORTS_256_PALETTE   TRUE
	#define QUANTUM_PAINTER_SUPPORTS_NATIVE_COLORS TRUE
	#define QUANTUM_PAINTER_NUM_IMAGES      16   // The maximum number of images/animations that can be loaded at any one time.
	#define QUANTUM_PAINTER_NUM_FONTS	    8	// The maximum number of fonts that can be loaded at any one time.
	#define QUANTUM_PAINTER_CONCURRENT_ANIMATIONS 2 //The maximum number of animations that can be executed at the same time.
	#define QUANTUM_PAINTER_DEBUG	     unset  // Prints out significant amounts of debugging information to CONSOLE output. Significant performance degradation, use only for debugging.
	#define QUANTUM_PAINTER_PIXDATA_BUFFER_SIZE	1024	//The limit of the amount of pixel data that can be transmitted in one transaction to the display. Higher values require more RAM on the MCU.

#endif // defined(QUANTUM_PAINTER_ENABLE)
	
/*** PWM DRIVER DEFINITIONS ***/
// There is no official API for PWM, so PWM Backlight is used
// GPIO8 - PWM4 A
#define BACKLIGHT_PWM_DRIVER    PWMD4
#define BACKLIGHT_PWM_CHANNEL   RP2040_PWM_CHANNEL_A
// #define BACKLIGHT_PAL_MODE      (PAL_MODE_ALTERNATE_PWM | PAL_RP_PAD_DRIVE12 | PAL_RP_GPIO_OE)
#define BACKLIGHT_PAL_MODE      2
#define BACKLIGHT_DEFAULT_LEVEL 10 // defined in keyboard.json
