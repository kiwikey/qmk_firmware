#pragma once

#include "quantum.h"

#define SCREENSAVER_STARRY_COUNT     24
#define SCREENSAVER_STARRY_FONT      font_oled
#define SCREENSAVER_STARRY_FADE_STEP 32 // brightness (V) change per step - 255/32 ~= 8 steps from off to full
#define SCREENSAVER_STARRY_SPEED_MIN 2  // frames per brightness step - lower = faster twinkle
#define SCREENSAVER_STARRY_SPEED_MAX 8

void screensaver_starry_night_init(bool initial);
void screensaver_starry_night_step(void);
