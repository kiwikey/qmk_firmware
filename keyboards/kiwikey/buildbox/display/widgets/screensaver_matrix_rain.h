#pragma once

#include "quantum.h"

// "matrix_rain_1" (full alphanumeric) and "matrix_rain_2" ("0"/"1" only) are the
// same falling-column effect, just seeded with a different charset - they share
// state and step(); only their init() differs.

#define SCREENSAVER_MATRIX_TRAIL_MIN   4  // rows behind the head that stay lit
#define SCREENSAVER_MATRIX_TRAIL_MAX   14
#define SCREENSAVER_MATRIX_SPEED_MIN   2  // frames between steps - lower = faster
#define SCREENSAVER_MATRIX_SPEED_MAX   7

#define SCREENSAVER_MATRIX_HEAD_COLOR  HSV_WHITE     // newest (leading) glyph of each drop
#define SCREENSAVER_MATRIX_TRAIL_COLOR 85, 255, 130  // dimmer green - same hue as HSV_GREEN, lower value

void screensaver_matrix_rain_1_init(bool initial);
void screensaver_matrix_rain_2_init(bool initial);
void screensaver_matrix_rain_step(void);
