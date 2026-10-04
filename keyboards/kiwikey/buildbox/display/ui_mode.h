#pragma once

#include "quantum.h"

/*** What owns the screen - and with it the 2 buttons and the knob - right now.
	Exactly one mode is active at a time; this replaces the separate menu_state /
	"is_active" flags each screen used to keep for itself. Every screen switch goes
	through ui_set_mode(), which also does the bookkeeping common to all of them
	(see ui_mode.c). Button and knob routing per mode is table-driven:
	ui_button_actions[] (qp_graphics.c) and knob_step_actions[] (sensors_handler.c).
	The LCD-Timeout backlight sleep (display_is_asleep()) is NOT a mode - the screen
	content stays whatever mode it was in, only the backlight is off.
***/
typedef enum {
	UI_MODE_BOOT = 0,           // boot animation playing / display not up yet - all input ignored
	UI_MODE_IDLE,               // the idle screen: layer box, keymap grid, status, knob

	// Settings Menu pages - everything from UI_MODE_MENU_LIST to UI_MODE_MENU_DEBUG
	// counts as "in the menu" (ui_mode_is_menu())
	UI_MODE_MENU_LIST,          // main "SETTINGS" list
	UI_MODE_MENU_EDIT,          // editing one main-list value in place
	UI_MODE_DIAL_LIST,          // "DIAL SETTINGS" sub-page
	UI_MODE_DIAL_EDIT,          // editing one DIAL SETTINGS value
	UI_MODE_LAYERS_LIST,        // "LAYERS CONFIG" sub-page
	UI_MODE_LAYERS_PICK,        // LAYERS CONFIG icon picker
	UI_MODE_MENU_ABOUT,         // "ABOUT BUILDBOX" screen
	UI_MODE_MENU_DFU_CONFIRM,   // "BOOT TO DFU" confirmation screen
	UI_MODE_MENU_DEBUG,         // debug info screen (menu item currently disabled)

	// Other full-screen modes
	UI_MODE_BREAKOUT,
	UI_MODE_TUTORIAL,
	UI_MODE_SCREENSAVER,

	UI_MODE_COUNT // keep last
} ui_mode_t;

ui_mode_t ui_get_mode(void);

// Switch modes. The caller draws the new screen itself, except UI_MODE_IDLE,
// which ui_set_mode() redraws (ui_refresh()) whenever it's entered from another mode.
void ui_set_mode(ui_mode_t mode);

bool ui_mode_is_menu(ui_mode_t mode);

// True only while the idle screen is what's on the panel. Every redraw that can be
// triggered from the background (VIA, layer changes, sensors, webhid) must check
// this before drawing; whatever covers the idle screen redraws it when it closes.
bool ui_idle_screen_visible(void);
