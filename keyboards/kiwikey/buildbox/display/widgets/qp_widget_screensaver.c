#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp_widget_screensaver.h"

#include <stdlib.h>
#include "timer.h"
#include "features/eeprom_custom.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/widgets/qp_menu.h"
#include "display/widgets/qp_widget_breakout.h"
#include "display/widgets/tutorial.h"
#include "display/widgets/screensaver_matrix_rain.h"
#include "display/widgets/screensaver_sleeping_zzz.h"
#include "display/widgets/screensaver_starry_night.h"

// eepdata.screensaver_effect (1-based; see SCREENSAVER_OFF_INDEX below) picks
// one of these by index (see screensaver_start() below and the "BURN-IN
// PROTECTION" menu item, qp_menu.c) - shown to the user via
// screen_saver_effect_list[] (qp_menu.h), never these internal names.
static const screensaver_effect_t effects[] = {
	{"matrix_rain_1", screensaver_matrix_rain_1_init, screensaver_matrix_rain_step},
	{"matrix_rain_2", screensaver_matrix_rain_2_init, screensaver_matrix_rain_step},
	{"sleeping_zzz",  screensaver_sleeping_zzz_init,  screensaver_sleeping_zzz_step},
	{"starry_night",  screensaver_starry_night_init,  screensaver_starry_night_step},
};
#define SCREENSAVER_EFFECT_COUNT (sizeof(effects) / sizeof(effects[0]))

// eepdata.screensaver_effect is 1-based here: 0 means "OFF" (screen_saver_effect_list[0],
// qp_menu.h), and 1..SCREENSAVER_EFFECT_COUNT map to effects[0..SCREENSAVER_EFFECT_COUNT-1].
#define SCREENSAVER_OFF_INDEX 0

static bool                        active = false;
static const screensaver_effect_t *current_effect;
static uint32_t                    last_frame;

bool screensaver_is_active(void) {
	return active;
}

uint8_t screensaver_effect_count(void) {
	return SCREENSAVER_EFFECT_COUNT;
}

static void screensaver_start(void) {
	static bool seeded = false;
	if (!seeded) { // varies each effect's own internal randomization (column positions, star timing, ...) across power-ons
		srand(timer_read32());
		seeded = true;
	}

	// eepdata.screensaver_effect is 1-based (0 = OFF, already filtered out by
	// housekeeping_task_screensaver() before this ever runs) - shift to the 0-based effects[] index
	uint8_t idx    = (eepdata.screensaver_effect >= 1 && eepdata.screensaver_effect <= SCREENSAVER_EFFECT_COUNT)
	                 ? eepdata.screensaver_effect - 1 : 0;
	active         = true;
	current_effect = &effects[idx];
	current_effect->init(true);
	last_frame = timer_read32();
}

void screensaver_exit(void) {
	active = false;
	ui_refresh();
}

void housekeeping_task_screensaver(void) {
	if (!active) {
		if (eepdata.screensaver_effect == SCREENSAVER_OFF_INDEX) return; // user turned it off
		if (eepdata.display_timeout < DISPLAY_TIMEOUT_1HOUR_INDEX) return; // only a screensaver on "1 Hour"/NEVER, where the backlight otherwise never (or rarely) sleeps on its own
		if (menu_state != NOT_IN_MENU || breakout_is_active() || tutorial_is_active()) return; // never hijack another full-screen mode
		if (last_input_activity_elapsed() < SCREENSAVER_IDLE_MS) return;
		screensaver_start();
		return;
	}

	if (timer_elapsed32(last_frame) < SCREENSAVER_FRAME_MS) return;
	last_frame = timer_read32();

	current_effect->step();
	qp_flush(my_display);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
