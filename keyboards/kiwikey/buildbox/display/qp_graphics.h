#pragma once

#include "features/webhid_shadow.h" // redirects qp_rect/qp_flush/etc to also mirror over webhid_stream; see that header

#define BOOT_DURATION          4000 // ms

// eepdata.display_timeout is an INDEX into these two, not a raw seconds value -
// cycled in process_encoder_rotate() (sensors_handler.c) via MENU_DISPLAYTIMEOUT,
// same pattern as eepdata.knob_func/knob_effect/screensaver_effect.
#define DISPLAY_TIMEOUT_COUNT       6
#define DISPLAY_TIMEOUT_NEVER_INDEX  (DISPLAY_TIMEOUT_COUNT - 1) // last entry = "NEVER"
#define DISPLAY_TIMEOUT_1HOUR_INDEX  (DISPLAY_TIMEOUT_COUNT - 2) // second-to-last entry = "1 Hour" - also arms the screensaver, see qp_widget_screensaver.c

static const uint32_t display_timeout_seconds[DISPLAY_TIMEOUT_COUNT] = {
	120,  // 2 min
	300,  // 5 min
	900,  // 15 min
	1800, // 30 min
	3600, // 1 Hour
	0,    // NEVER - unused; guarded by DISPLAY_TIMEOUT_NEVER_INDEX instead, see housekeeping_task_display()
};
static const char * const display_timeout_text[DISPLAY_TIMEOUT_COUNT] = {
	"2 min",
	"5 min",
	"15 min",
	"30 min",
	"1 hour",
	"NEVER"
};

extern painter_device_t my_display;
extern bool booting;
extern bool ui_refresh_pending;

void display_init(void);
void display_bootup(void);
void ui_refresh(void);

void keyboard_post_init_display(void);
void housekeeping_task_display(void);
bool process_record_display(uint16_t keycode, keyrecord_t *record);
bool display_is_asleep(void); // true while the backlight is zeroed for the idle timeout - see housekeeping_task_display()

void test_fonts(void);
