#pragma once

#define FW_VERSION "2.0.250716"

#if defined(CONSOLE_ENABLE)
	#define DEBUG_MATRIX_SCAN_RATE
#endif // defined(CONSOLE_ENABLE)

#if defined(VIA_ENABLE)
	#define DYNAMIC_KEYMAP_LAYER_COUNT 6
	#define VIA_EEPROM_CUSTOM_CONFIG_SIZE 19
#endif // defined(VIA_ENABLE)

#define OLED_DISPLAY_128X64
#define OLED_TIMEOUT 0 // the OLED timeout is manually controlled by custom firmware
#define OLED_FONT_H "../0_libraries/custom_display/oled/glcdfont.c"

// AVR firmware size reducing
#define NO_ACTION_ONESHOT      // 636 bytes
#define NO_ACTION_TAPPING      // 1558 bytes, considerable
#undef LOCKING_SUPPORT_ENABLE  // not sure
#undef LOCKING_RESYNC_ENABLE   // not sure
