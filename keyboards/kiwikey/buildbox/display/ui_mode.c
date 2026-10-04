#include "ui_mode.h"

#include "features/eeprom_custom.h"
#include "sensor/sensors_handler.h"
#include "display/qp_graphics.h"

static ui_mode_t current_mode = UI_MODE_BOOT;

ui_mode_t ui_get_mode(void) {
	return current_mode;
}

bool ui_mode_is_menu(ui_mode_t mode) {
	return mode >= UI_MODE_MENU_LIST && mode <= UI_MODE_MENU_DEBUG;
}

bool ui_idle_screen_visible(void) {
	return display_ready && current_mode == UI_MODE_IDLE;
}

void ui_set_mode(ui_mode_t mode) {
	ui_mode_t old_mode = current_mode;
	current_mode       = mode;

	// Rotation left over from the previous screen never carries into the new one
	// (it would show up as a "weird cursor jump" or a burst of volume taps).
	knob_accumulator = 0;

	// Leaving the Settings Menu - by Exit, or into Breakout/tutorial, or any other
	// path - persists whatever was changed in it.
	if (ui_mode_is_menu(old_mode) && !ui_mode_is_menu(mode)) {
		eeprom_custom_save();
	}

	// Coming back to the idle screen from anything else always redraws it in full.
	if (mode == UI_MODE_IDLE && old_mode != UI_MODE_IDLE) {
		ui_refresh();
	}
}
