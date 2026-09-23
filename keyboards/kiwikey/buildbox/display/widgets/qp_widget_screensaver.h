#pragma once

#include "quantum.h"
#include "display/defines.h"

/***  Full-screen idle-time effects, picked at random each time the screensaver
	activates. Only arms while the LCD Timeout (eepdata.display_timeout) is set
	to NEVER - otherwise the ordinary backlight-off idle timeout in
	housekeeping_task_display() already handles the idle screen, so the two
	never run at the same time.

	Each effect lives in its own screensaver_<name>.c/.h (see those files) and
	registers itself in the `effects[]` table in qp_widget_screensaver.c.
***/

#define SCREENSAVER_IDLE_MS   30000 // ms of no input before it kicks in
#define SCREENSAVER_FRAME_MS  40    // ms between animation ticks

// Shared glyph grid most effects draw on (font_oled is fixed-width)
#define SCREENSAVER_FONT      font_oled
#define SCREENSAVER_GLYPH_W   6
#define SCREENSAVER_GLYPH_H   8
#define SCREENSAVER_COLS      (ST7789_WIDTH  / SCREENSAVER_GLYPH_W)
#define SCREENSAVER_ROWS      (ST7789_HEIGHT / SCREENSAVER_GLYPH_H)

/***  One screensaver effect:
	init(initial) - (re)seeds its state and clears the screen. 'initial' spreads
	                 the effect across the whole screen so it looks already
	                 running instead of starting from blank; called once, when
	                 this effect is picked for the current activation.
	step()        - advances the effect by one animation tick. Called by
	                 housekeeping_task_screensaver() on the shared frame timer;
	                 qp_flush() is handled by the caller, not by step() itself.
***/
typedef struct {
	const char *name;
	void (*init)(bool initial);
	void (*step)(void);
} screensaver_effect_t;

bool    screensaver_is_active(void);
void    screensaver_exit(void);              // any input while active: close it, back to the idle screen
void    housekeeping_task_screensaver(void); // arms it after SCREENSAVER_IDLE_MS idle, ticks the active effect while active
uint8_t screensaver_effect_count(void);      // number of registered effects - see effects[] in qp_widget_screensaver.c
