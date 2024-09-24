#ifndef OLED_H
#define OLED_H

#include "quantum.h"

#define DISPLAY_TIMEOUT_MIN    30	// seconds
#define DISPLAY_TIMEOUT_STEP   30
#define DISPLAY_TIMEOUT_MAX    180
#define DISPLAY_TIMEOUT_NEVER  DISPLAY_TIMEOUT_MAX + DISPLAY_TIMEOUT_STEP

bool process_record_oled(uint16_t keycode, keyrecord_t *record);

#endif /* OLED_H */
