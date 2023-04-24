// Copyright 2023 KiwiKey (@KiwiKey)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define WS2812_PIO_USE_PIO1 // Force the usage of PIO1 peripheral, by default the WS2812 implementation uses the PIO0 peripheral

/*** SPI DRIVER DEFINITIONS ***/
#define SPI_DRIVER        SPID0
#define SPI_SCK_PIN       GP18 // SCK
#define SPI_MOSI_PIN      GP19 // MOSI
#define SPI_MISO_PIN      GP20 // MISO
//#define SPI_SCK_PAL_MODE  5
//#define SPI_MOSI_PAL_MODE 5
//#define SPI_MISO_PAL_MODE 5
#define DISPLAY_SPI_DIVISOR 1 // 1, 2, 4, 8 all works
#define DISPLAY_SPI_MODE    3 // ST7789 240*240 works with mode 2 or 3 only, not sure why

/*** QUANTUM PAINTER DRIVER DEFINITIONS ***/
#define ST7789_WIDTH  240
#define ST7789_HEIGHT 240

//#define DISPLAY_CS_PIN      GP17 // cannot be changed
//#define DISPLAY_DC_PIN      GP16
//#define DISPLAY_RST_PIN     GP20

#define DISPLAY_CS_PIN      GP2
#define DISPLAY_DC_PIN      GP1
#define DISPLAY_RST_PIN     GP0

#define QUANTUM_PAINTER_NUM_IMAGES	4	//The maximum number of images/animations that can be loaded at any one time.
#define QUANTUM_PAINTER_NUM_FONTS	2	//The maximum number of fonts that can be loaded at any one time.
#define QUANTUM_PAINTER_CONCURRENT_ANIMATIONS	2	//The maximum number of animations that can be executed at the same time.
#define QUANTUM_PAINTER_LOAD_FONTS_TO_RAM	FALSE	//Whether or not fonts should be loaded to RAM. Relevant for fonts stored in off-chip persistent storage, such as external flash.
#define QUANTUM_PAINTER_PIXDATA_BUFFER_SIZE	1024	//The limit of the amount of pixel data that can be transmitted in one transaction to the display. Higher values require more RAM on the MCU.
#define QUANTUM_PAINTER_SUPPORTS_256_PALETTE	FALSE	//If 256-color palettes are supported. Requires significantly more RAM on the MCU.
#define QUANTUM_PAINTER_DEBUG	unset	//Prints out significant amounts of debugging information to CONSOLE output. Significant performance degradation, use only for debugging.
