#pragma once

#include "quantum.h"

#define DYNAMIC_KEYMAP_EEPROM_ADDR   VIA_EEPROM_CUSTOM_CONFIG_ADDR+VIA_EEPROM_CUSTOM_CONFIG_SIZE // This should be pre-defined in QMK?

// Give the USB power rail time to settle before touching any hardware
// (backlight PWM, TFT SPI init, AS5600 I2C init) - avoids boot-time glitches
// caused by initializing peripherals while the rail is still stabilizing.
#define POST_INIT_POWER_SETTLE_MS 100
