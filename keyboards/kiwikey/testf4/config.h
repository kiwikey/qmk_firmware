#pragma once

#define ST7789_WIDTH  240
#define ST7789_HEIGHT 240

#define SPI_DRIVER        SPID2
#define SPI_SCK_PIN       B13 // SCK
#define SPI_MOSI_PIN      B15 // MOSI
#define SPI_MISO_PIN      B14 // MISO
#define SPI_SCK_PAL_MODE  5
#define SPI_MOSI_PAL_MODE 5
#define SPI_MISO_PAL_MODE 5

#define DISPLAY_CS_PIN      B9 // cannot be changed
#define DISPLAY_DC_PIN      B12
#define DISPLAY_RST_PIN     B14
#define DISPLAY_SPI_DIVISOR 1 // 1, 2, 4, 8 all works
#define DISPLAY_SPI_MODE    3 // ST7789 240*240 works with mode 2 or 3 only, not sure why

#define QUANTUM_PAINTER_NUM_IMAGES	8	//The maximum number of images/animations that can be loaded at any one time.
#define QUANTUM_PAINTER_NUM_FONTS	4	//The maximum number of fonts that can be loaded at any one time.
#define QUANTUM_PAINTER_CONCURRENT_ANIMATIONS	4	//The maximum number of animations that can be executed at the same time.
#define QUANTUM_PAINTER_LOAD_FONTS_TO_RAM	FALSE	//Whether or not fonts should be loaded to RAM. Relevant for fonts stored in off-chip persistent storage, such as external flash.
#define QUANTUM_PAINTER_PIXDATA_BUFFER_SIZE	32	//The limit of the amount of pixel data that can be transmitted in one transaction to the display. Higher values require more RAM on the MCU.
#define QUANTUM_PAINTER_SUPPORTS_256_PALETTE	FALSE	//If 256-color palettes are supported. Requires significantly more RAM on the MCU.
//#define QUANTUM_PAINTER_DEBUG	unset	//Prints out significant amounts of debugging information to CONSOLE output. Significant performance degradation, use only for debugging.

#define TAP_CODE_DELAY 10
/* Mechanical locking support. Use KC_LCAP", "KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE
