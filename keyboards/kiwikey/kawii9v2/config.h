// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define OLED_DISPLAY_128X64
#define OLED_TIMEOUT 0 // the OLED timeout is controlled by custom firmware
#define OLED_FONT_H "lib/glcdfont.c"

#define DYNAMIC_KEYMAP_LAYER_COUNT 6
// #define VIA_EEPROM_CUSTOM_CONFIG_ADDR ?  // need to dig in
#define VIA_EEPROM_CUSTOM_CONFIG_SIZE 9

#define FW_VERSION "2.0.240609"

// Try to save some bytes...
#define NO_ACTION_ONESHOT      // 636 bytes
#define NO_ACTION_TAPPING      // 1558 bytes, considerable
#undef LOCKING_SUPPORT_ENABLE  // not sure
#undef LOCKING_RESYNC_ENABLE   // not sure
