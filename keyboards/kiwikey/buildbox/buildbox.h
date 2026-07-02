#pragma once

#include "quantum.h"

#if defined(QUANTUM_PAINTER_ENABLE)
	#include "qp.h"
	extern painter_device_t my_display;
#endif // defined(QUANTUM_PAINTER_ENABLE)

#define DYNAMIC_KEYMAP_EEPROM_ADDR     VIA_EEPROM_CUSTOM_CONFIG_ADDR+VIA_EEPROM_CUSTOM_CONFIG_SIZE // This should be pre-defined in QMK?

bool process_record_display(uint16_t keycode, keyrecord_t *record);
