#pragma once

#define FW_VERSION "2.0.261013"

#if defined(CONSOLE_ENABLE)
	#define DEBUG_MATRIX_SCAN_RATE
#endif // defined(CONSOLE_ENABLE)

#if defined(VIA_ENABLE)
	#define DYNAMIC_KEYMAP_LAYER_COUNT 6
	#define VIA_EEPROM_CUSTOM_CONFIG_SIZE 21 // must equal sizeof(EEPROM_CUSTOM_DATA), see static assert in kawii9v2.c
#endif // defined(VIA_ENABLE)

#define OLED_IC            OLED_IC_SH1106
#define OLED_COLUMN_OFFSET 2

#define OLED_DISPLAY_128X64
#define OLED_TIMEOUT 0 // the OLED timeout is manually controlled by custom firmware
#define OLED_FONT_H "../0_libraries/custom_display/oled/glcdfont.c"

#define ENCODER_DIRECTION_FLIP

// AVR firmware size reducing
#define NO_ACTION_ONESHOT      // 636 bytes
#define NO_ACTION_TAPPING      // 1558 bytes, considerable
#undef LOCKING_SUPPORT_ENABLE  // not sure
#undef LOCKING_RESYNC_ENABLE   // not sure
