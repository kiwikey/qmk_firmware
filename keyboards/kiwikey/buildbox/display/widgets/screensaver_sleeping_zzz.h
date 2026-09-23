#pragma once

#include "quantum.h"

#define SCREENSAVER_ZZZ_COUNT      10  // independent "Z" particles
#define SCREENSAVER_ZZZ_SPEED_MIN  3   // frames per upward step - lower = faster
#define SCREENSAVER_ZZZ_SPEED_MAX  10
#define SCREENSAVER_ZZZ_COLOR      HSV_AZURE     // pale, sleepy blue
#define SCREENSAVER_ZZZ_FONT_SMALL font_oled
#define SCREENSAVER_ZZZ_FONT_BIG   font16        // alternated in, for a "staggered chain" of different-sized Z's

void screensaver_sleeping_zzz_init(bool initial);
void screensaver_sleeping_zzz_step(void);
