#include "sensors_handler.h"

#include <lib/lib8tion/lib8tion.h>

#include "features/eeprom_custom.h"
#include "features/knob_custom.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"
#include "display/widgets/qp_menu.h"
#include "display/widgets/qp_widget_breakout.h"
#include "display/widgets/qp_widget_screensaver.h"
#include "display/widgets/tutorial.h"

static const uint16_t knob_sensitivity_step[] = {
	256, // LOW
	128, // MEDIUM
	64,  // HIGH
};
_Static_assert(sizeof(knob_sensitivity_step) / sizeof(knob_sensitivity_step[0]) == (KNOB_SENSITIVITY_COUNT), "knob_sensitivity_step must have exactly KNOB_SENSITIVITY_COUNT entries");

int16_t knob_accumulator = 0;
uint32_t last_knob_movement_time = 0;

static void knob_on_rotation(bool direction, uint16_t distance);

void keyboard_post_init_sensors_handler(void) {
	keyboard_post_init_magnetic_encoder();
}

void housekeeping_task_sensors_handler(void) {
	// -1 = not yet synced with the sensor's actual boot-time state
	static int8_t magnet_was_present = -1;

	int8_t movement = housekeeping_task_magnetic_encoder(); // -1 CCW, 0 none, +1 CW
	if (movement != 0) {
		knob_on_rotation(movement > 0, magnetic_encoder.last_distance);
	}

	if (magnet_was_present != (int8_t)magnetic_encoder.is_present) {
		magnet_was_present = magnetic_encoder.is_present;
		// The knob widget is only visible on the idle screen; whatever covers it
		// redraws it correctly (via ui_refresh -> widget_knob_init) when it closes,
		// so skip poking the display while something else is showing.
		if (ui_idle_screen_visible()) {
			magnet_was_present ? widget_knob_show_dot() : widget_knob_show_missing();
		}
	}
}

// Knob on the idle screen: move the on-screen dot, and send the knob function's
// keycode (volume / scroll) once per eepdata.knob_sensitivity worth of rotation.
static void knob_idle_rotation(void) {
	widget_knob_update(magnetic_encoder.prev_angle, magnetic_encoder.new_angle);

	uint16_t code_cw = KC_NO, code_ccw = KC_NO;
	switch (eepdata.knob_func) {
		case KNOB_FUNC_HSCROLL: code_cw = MS_WHLR; code_ccw = MS_WHLL; break;
		case KNOB_FUNC_VSCROLL: code_cw = MS_WHLD; code_ccw = MS_WHLU; break;
		case KNOB_FUNC_VOLUME:  code_cw = KC_VOLU; code_ccw = KC_VOLD; break;
		default: break; // KNOB_FUNC_CUSTOM - no built-in action yet
	}

	// Each tap_code16() blocks for TAP_CODE_DELAY, so cap the taps per sensor
	// event and drop whatever rotation is left beyond that - a fast spin then
	// can't stall the matrix scan, or leave a backlog that fires later.
	int16_t sensitivity_threshold = knob_sensitivity_step[eepdata.knob_sensitivity]; // range-checked by eeprom_custom_validate()
	uint8_t taps = 0;
	while (knob_accumulator >= sensitivity_threshold && taps < KNOB_MAX_TAPS_PER_EVENT) {
		if (code_cw != KC_NO) tap_code16(code_cw);
		knob_accumulator -= sensitivity_threshold;
		taps++;
	}
	while (knob_accumulator <= -sensitivity_threshold && taps < KNOB_MAX_TAPS_PER_EVENT) {
		if (code_ccw != KC_NO) tap_code16(code_ccw);
		knob_accumulator += sensitivity_threshold;
		taps++;
	}
	if (knob_accumulator >= sensitivity_threshold)  knob_accumulator = sensitivity_threshold - 1;
	if (knob_accumulator <= -sensitivity_threshold) knob_accumulator = -(sensitivity_threshold - 1);
}

// Knob routing for every full-screen mode that moves in fixed-size detents: one
// on_step() call per step_size of rotation. Modes not listed (zeroed) ignore the
// knob, except UI_MODE_IDLE and UI_MODE_TUTORIAL, handled in knob_on_rotation().
// A new full-screen mode only needs a row here (and in ui_button_actions[],
// display/qp_graphics.c).
typedef struct {
	int16_t step_size;              // sensor counts per step (4096 = one full turn)
	void  (*on_step)(bool clockwise);
} knob_step_action_t;

static const knob_step_action_t knob_step_actions[UI_MODE_COUNT] = {
	[UI_MODE_MENU_LIST ... UI_MODE_LAYERS_PICK] = { MENU_STEP_SIZE,     menu_process_rotation },  // every Settings Menu page - menu_process_rotation() dispatches by mode
	[UI_MODE_BREAKOUT]                          = { BREAKOUT_STEP_SIZE, breakout_encoder_tick },  // finer steps for the paddle
};

// Not in any public header, but has external linkage in quantum/keyboard.c.
// Our knob bypasses QMK's ENCODER_ENABLE pipeline entirely (custom AS5600 I2C
// read), so nothing else marks rotation as "activity" for last_input_activity_elapsed()
// (LCD Timeout + screensaver, qp_graphics.c/qp_widget_screensaver.c) unless we do it ourselves.
extern void last_encoder_activity_trigger(void);

// Called from housekeeping_task_sensors_handler() once the AS5600 driver has
// read and validated a movement past DEG_MARGIN_AS5600. `direction` is its
// sign (true = CW, false = CCW) and `distance` its size in sensor counts.
// Doesn't touch the sensor itself, avoiding a second, racy I2C read per tick.
static void knob_on_rotation(bool direction, uint16_t distance) {
	// Same rule as the first keypress (process_record_display(), qp_graphics.c):
	// while the LCD Timeout has the display asleep, rotation only wakes it -
	// housekeeping_task_display() turns the backlight back on next tick - and
	// is not treated as input (no volume/scroll change the user can't see).
	if (display_is_asleep()) {
		last_encoder_activity_trigger();
		knob_accumulator = 0;
		return;
	}

	last_encoder_activity_trigger();
	last_knob_movement_time = timer_read32();

	if (screensaver_is_active()) { // any rotation just dismisses it, same as a keypress
		screensaver_exit();
		return;
	}

	// Clamped: not every state drains it (tutorial knob screen, debug/DFU/About
	// screens), and an int16_t overflow there would be undefined behavior.
	int32_t sum = (int32_t)knob_accumulator + (direction ? (int32_t)distance : -(int32_t)distance);
	if (sum >  KNOB_ACCUMULATOR_LIMIT) sum =  KNOB_ACCUMULATOR_LIMIT;
	if (sum < -KNOB_ACCUMULATOR_LIMIT) sum = -KNOB_ACCUMULATOR_LIMIT;
	knob_accumulator = (int16_t)sum;

	ui_mode_t mode = ui_get_mode();
	switch (mode) {
		case UI_MODE_IDLE:
			knob_idle_rotation();
			break;
		case UI_MODE_TUTORIAL:
			// Screen navigation is buttons-only - but the knob screen shows a live dot
			// and the menu-practice screens move a cursor, so tutorial.c consumes
			// knob_accumulator itself (tutorial_knob_rotated() is a no-op elsewhere).
			tutorial_knob_rotated();
			break;
		default: {
			// Fixed-size detents for every other mode - see knob_step_actions[] above
			const knob_step_action_t *action = &knob_step_actions[mode];
			if (!action->on_step) break; // static screens (About, DFU confirm, debug) ignore the knob
			while (knob_accumulator >= action->step_size && ui_get_mode() == mode) {
				action->on_step(CW);
				knob_accumulator -= action->step_size;
			}
			while (knob_accumulator <= -action->step_size && ui_get_mode() == mode) {
				action->on_step(CCW);
				knob_accumulator += action->step_size;
			}
			break;
		}
	}
}

