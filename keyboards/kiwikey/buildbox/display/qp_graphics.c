#include QMK_KEYBOARD_H
#include "qp_graphics.h"

#include "features/eeprom_custom.h"
#include "sensor/sensors_handler.h"
#include "display/qp_includes.h"
#include "display/resources/graphics/gif_bootup01.qgf.h" // gfx_gif_bootup01 - loaded on demand, see keyboard_post_init_display()
#include "display/qp_custom_api.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"
#include "display/widgets/qp_widget_status.h"
#include "display/widgets/qp_menu.h"
#include "display/widgets/qp_widget_breakout.h"
#include "display/widgets/qp_widget_screensaver.h"
#include "display/widgets/tutorial.h"

const uint32_t display_timeout_seconds[] = {
	120,  // 2 min
	300,  // 5 min
	900,  // 15 min
	1800, // 30 min
	3600, // 1 Hour
	0,    // NEVER - unused; guarded by DISPLAY_TIMEOUT_NEVER_INDEX instead, see housekeeping_task_display()
};
_Static_assert(sizeof(display_timeout_seconds) / sizeof(display_timeout_seconds[0]) == (DISPLAY_TIMEOUT_COUNT), "display_timeout_seconds must have exactly DISPLAY_TIMEOUT_COUNT entries");

const char * const display_timeout_text[] = {
	"2 min",
	"5 min",
	"15 min",
	"30 min",
	"1 hour",
	"NEVER"
};
_Static_assert(sizeof(display_timeout_text) / sizeof(display_timeout_text[0]) == (DISPLAY_TIMEOUT_COUNT), "display_timeout_text must have exactly DISPLAY_TIMEOUT_COUNT entries");

painter_device_t bb_display;
bool     	display_ready = false;
static bool display_asleep = false;
static bool rgb_was_enabled_before_sleep = false; // see housekeeping_task_display()'s LCD Timeout block

bool display_is_asleep(void) {
	return display_asleep;
}

uint16_t flag_display_keycode_changed = 0x0000;
// flag_display_keycode_changed: contains layer, row, col of changed key
// Mask:     00          00      00       00
//       is changed?   layer     row      col
// Example: 0x1231 = changed, layer 2, row 3, col 1

bool flag_display_keymap_reload = false;
// flag_display_keymap_reload: set when VIA changes many keys at once (layout load,
// keymap reset) or more than one key between two housekeeping ticks - redraws the whole grid

uint8_t flag_widget_layer_changed = 0;
// 0 = nothing changed (we need this, so other layers need to +1)
// 1 = layer 0 changed
// 2 = layer 1 changed

// RGB Matrix state, polled in housekeeping_task_display() so the status widget
// catches every way it can change - a keycode, VIA's Lighting panel (which
// applies the change directly via raw HID, bypassing process_record entirely),
// or anything else - rather than trying to intercept each trigger individually.
static bool    rgb_status_last_enabled = false;
static uint8_t rgb_status_last_mode    = 0;
static uint8_t rgb_status_last_val     = 0;

// "DIAL SETTINGS" shortcut: on the idle screen, Button 1 held this long jumps
// straight to dial_menu_open() (qp_menu.c) instead of its normal previous-layer
// tap action. Checked in housekeeping_task_display(), so it fires the moment
// the threshold is reached instead of waiting for release; tracked (and the
// tap action suppressed once it fires) in process_record_display().
// DIAL_SETTINGS_HOLD_MS lives in display/defines.h.
static bool     button1_held       = false;
static uint32_t button1_press_time = 0;

void display_init(void) {
#if defined(QUANTUM_PAINTER_ILI9341_SPI_ENABLE)
	bb_display = qp_ili9341_make_spi_device(
		ILI9341_WIDTH,
		ILI9341_HEIGHT,
		DISPLAY_CS_PIN,
		DISPLAY_DC_PIN,
		DISPLAY_RST_PIN,
		DISPLAY_SPI_DIVISOR,
		DISPLAY_SPI_MODE
	);
#elif defined(QUANTUM_PAINTER_ST7789_SPI_ENABLE)
	bb_display = qp_st7789_make_spi_device(
		ST7789_WIDTH,
		ST7789_HEIGHT,
		DISPLAY_CS_PIN,
		DISPLAY_DC_PIN,
		DISPLAY_RST_PIN,
		DISPLAY_SPI_DIVISOR,
		DISPLAY_SPI_MODE
	);
#endif
	qp_init(bb_display, DISPLAY_ROTATION);   // Initialise bb_display

	qp_power(bb_display, true);
	qp_clear(bb_display);
	qp_rect(bb_display, 0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, GLOBAL_BG_COLOR, true);
	qp_flush(bb_display);
	qp_init_load_files();
	display_ready = true;
}

// What normally shows once boot is done - the idle screen, unless this is the
// very first boot (eepdata.unbox_tutorial), in which case the tutorial takes
// over the idle screen's spot instead. Shared by both boot-animation-enabled
// and -disabled paths below.
static void show_idle_screen(void) {
	if (eepdata.unbox_tutorial) {
		tutorial_start();
	} else {
		ui_set_mode(UI_MODE_IDLE); // draws the idle screen (ui_refresh())
	}
}

uint32_t finish_boot_animation(uint32_t trigger_time, void *cb_arg) {
	qp_stop_animation(bb_boot_anim);
	// The boot GIF is only ever shown here - give its image slot back
	// (QUANTUM_PAINTER_NUM_IMAGES) instead of keeping it resident.
	qp_close_image(gif_bootup01);
	gif_bootup01 = NULL;
	show_idle_screen(); // leaves UI_MODE_BOOT - knob rotation during the animation is discarded by ui_set_mode()
	return 0;   // Don't schedule again
}

void keyboard_post_init_display(void) {
	display_init(); // ui_get_mode() is still UI_MODE_BOOT - all input ignored until show_idle_screen()

	if (eepdata.display_bootanim == 1) {
		gif_bootup01 = qp_load_image_mem(gfx_gif_bootup01); // loaded on demand, closed again in finish_boot_animation()
		bb_boot_anim = qp_animate(bb_display, 0, 90, gif_bootup01);
		defer_exec(BOOT_DURATION, finish_boot_animation, NULL);
	} else {
		show_idle_screen();
	}
}

void ui_refresh(void) {
	qp_rect(bb_display, 0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, GLOBAL_BG_COLOR, true); // Fill screen by black color
	qp_flush(bb_display);
	widget_matrix_init();
	widget_layer_init();
	widget_status_init();
	widget_knob_init();
	widget_matrix_keymap_render(get_highest_layer(layer_state));
	qp_flush(bb_display);
}

void housekeeping_task_display(void) { // Check all flags
	// VIA redraw requests (features/via_custom.c) are always consumed, but only drawn
	// while the idle screen is showing - whatever covers it (menu, Breakout, tutorial,
	// screensaver) calls ui_refresh() when it closes, which redraws from live data anyway.
	bool    idle_visible  = ui_idle_screen_visible();
	uint8_t current_layer = get_highest_layer(layer_state);

	if (flag_display_keymap_reload) { // bulk keymap change (layout load/reset) - redraw the whole grid
		if (idle_visible && current_layer < DYNAMIC_KEYMAP_LAYER_COUNT) {
			widget_matrix_keymap_render(current_layer);
		}
		flag_display_keymap_reload   = false;
		flag_display_keycode_changed = 0x0000; // already covered by the full redraw
	}

	if (flag_display_keycode_changed & 0x1000) {
		uint16_t layer = (flag_display_keycode_changed & 0x0F00) >> 8;
		uint16_t row   = (flag_display_keycode_changed & 0x00F0) >> 4;
		uint16_t col   = flag_display_keycode_changed & 0x00F;
		// only if that changed layer is the one showing, and only the 4x4 grid -
		// row 4 (the 2 direct-pin buttons) has no on-screen key box
		if (idle_visible && layer == current_layer && row < MATRIX_ROWS-1 && col < MATRIX_COLS) {
			widget_matrix_render_singlebutton(row,
											  col,
											  WIDGET_MATRIX_BUTTON_OFF,
											  true,
											  layer);
		}
		flag_display_keycode_changed = 0x0000;
	}

	if (flag_widget_layer_changed) { // 0 means nothing changed
		if (idle_visible && (flag_widget_layer_changed - 1) == current_layer) {
			widget_layer_render_layername(flag_widget_layer_changed - 1, WIDGET_LAYER_POSX, WIDGET_LAYER_POSY);
		}
		flag_widget_layer_changed = 0;
	}

	{
		bool    rgb_enabled = rgb_matrix_is_enabled();
		uint8_t rgb_mode    = rgb_matrix_get_mode();
		uint8_t rgb_val     = rgb_matrix_get_val();
		if (rgb_enabled != rgb_status_last_enabled || rgb_mode != rgb_status_last_mode || rgb_val != rgb_status_last_val) {
			rgb_status_last_enabled = rgb_enabled;
			rgb_status_last_mode    = rgb_mode;
			rgb_status_last_val     = rgb_val;
			// widget_status_update() draws at the idle screen's fixed position - only
			// safe to call there. RGB can still change while something else is up
			// (e.g. VIA's Lighting panel applies over raw HID, bypassing the
			// button/knob routing entirely - see rgb_status_last_* above), so keep tracking it either way;
			// ui_refresh() redraws it from live values anyway once we're back, so
			// skipping the draw here loses nothing.
			if (idle_visible) {
				widget_status_update();
			}
		}

		// "DIAL SETTINGS" shortcut: Button 1 held this long jumps straight to the sub-page
		if (idle_visible && button1_held && timer_elapsed32(button1_press_time) >= DIAL_SETTINGS_HOLD_MS) {
			button1_held = false;
			dial_menu_open(true); // via the shortcut, not the main list
		}
	}

	// LCD Timeout: cut the backlight after N seconds of no matrix/encoder activity.
	// (Using the backlight rather than qp_power(): DISPOFF stops the panel from
	// refreshing but doesn't blank the glass to black on this panel - it settles
	// to white instead. Zeroing the backlight makes it look off regardless.)
	// RGB matrix is disabled/re-enabled alongside it (noeeprom - doesn't touch the
	// user's saved on/off setting). rgb_was_enabled_before_sleep remembers whether
	// it was actually on going in, so waking up doesn't force it on if the user
	// had already turned it off themselves before the timeout hit.
	if (ui_get_mode() != UI_MODE_BOOT) {
		uint8_t  timeout_idx = eepdata.display_timeout; // range-checked by eeprom_custom_validate()
		uint32_t timeout_ms  = display_timeout_seconds[timeout_idx] * 1000UL;
		if (!display_asleep && timeout_idx != DISPLAY_TIMEOUT_NEVER_INDEX && last_input_activity_elapsed() >= timeout_ms) {
			backlight_level_noeeprom(0);
			rgb_was_enabled_before_sleep = rgb_matrix_is_enabled();
			rgb_matrix_disable_noeeprom();
			display_asleep = true;
		} else if (display_asleep && last_input_activity_elapsed() < timeout_ms) {
			backlight_level_noeeprom(eepdata.display_brightness);
			if (rgb_was_enabled_before_sleep) rgb_matrix_enable_noeeprom();
			display_asleep = false;
		}
	}
}

// What Button 1 / Button 2 do (on press) in every full-screen mode that just maps
// each button to one action. Modes not listed here either take no button input
// (UI_MODE_BOOT, UI_MODE_SCREENSAVER - see process_record_display()) or need
// press AND release (UI_MODE_TUTORIAL, UI_MODE_IDLE), handled separately below.
// A new full-screen mode only needs a row here (and in knob_step_actions[],
// sensor/sensors_handler.c, if it uses the knob).
typedef struct {
	void (*button1)(void);
	void (*button2)(void);
} ui_button_actions_t;

static const ui_button_actions_t ui_button_actions[UI_MODE_COUNT] = {
	// Settings Menu. On the in-place edit pages both buttons just end the edit
	// (values apply live while turning the knob); the icon picker is the one page
	// where Button 1 really cancels.
	[UI_MODE_MENU_LIST]        = { menu_exit,                menu_action               },
	[UI_MODE_MENU_EDIT]        = { menu_submenu_exit,        menu_submenu_exit         },
	[UI_MODE_DIAL_LIST]        = { dial_menu_exit,           dial_menu_action          },
	[UI_MODE_DIAL_EDIT]        = { dial_menu_submenu_exit,   dial_menu_submenu_exit    },
	[UI_MODE_LAYERS_LIST]      = { layers_menu_exit,         layers_menu_action        },
	[UI_MODE_LAYERS_PICK]      = { layers_menu_submenu_exit, layers_menu_submenu_save  },
	// Static screens opened from the main list: Button 1 leaves the menu,
	// Button 2 re-runs the item (a 2nd OK on BOOT TO DFU actually resets)
	[UI_MODE_MENU_ABOUT]       = { menu_exit,                menu_action               },
	[UI_MODE_MENU_DFU_CONFIRM] = { menu_exit,                menu_action               },
	[UI_MODE_MENU_DEBUG]       = { menu_exit,                menu_action               },
	// Breakout: Button 1 quits, Button 2 confirms difficulty / launches / restarts
	[UI_MODE_BREAKOUT]         = { breakout_exit,            breakout_button_action    },
};

// Tracks the one key whose press dismissed a passive idle state (backlight
// asleep, or the screensaver), so its matching release can be swallowed too
// (see the wake-up check in process_record_display()).
static bool    waking_press_pending = false;
static uint8_t waking_press_row, waking_press_col;

bool process_record_display(uint16_t keycode, keyrecord_t *record) {
	ui_mode_t mode = ui_get_mode();
	if (mode == UI_MODE_BOOT) return false;

	/*** If the display is asleep (idle timeout - see housekeeping_task_display())
		or the screensaver is showing: the first keypress just dismisses that
		passive state (wakes the backlight / closes the screensaver), it's not
		meant to act as input - the user is just reaching for the board, not
		intentionally using it yet. Swallow that one press and its matching
		release; every following key press behaves normally.
	***/
	if (record->event.pressed) {
		if (display_is_asleep() || mode == UI_MODE_SCREENSAVER) {
			if (mode == UI_MODE_SCREENSAVER) screensaver_exit();
			waking_press_pending = true;
			waking_press_row     = record->event.key.row;
			waking_press_col     = record->event.key.col;
			return false;
		}
	} else if (waking_press_pending &&
	           record->event.key.row == waking_press_row &&
	           record->event.key.col == waking_press_col) {
		waking_press_pending = false;
		return false;
	}

	/*** If the first-boot tutorial is showing :
		+ Pressing Button 1 -> Cancel (screen 0) / Prev (every screen after)
		+ Pressing Button 2 -> Next (or finish, on the last screen)
		+ TUTORIAL_SCREEN_MATRIX only: any other key still lights up on-screen,
		  same as the idle screen's widget_matrix_update() below - a live demo
		  of what real interactivity looks like. No keycode is ever actually
		  sent to the host during the tutorial (always returns false).
	***/
	if (mode == UI_MODE_TUTORIAL) {
		switch (keycode) {
			case KC_BUTTON_1:
				tutorial_button_action(false, record->event.pressed);
				break;
			case KC_BUTTON_2:
				tutorial_button_action(true, record->event.pressed);
				break;
			default:
				if (tutorial_matrix_demo_is_active()) {
					widget_matrix_update(record->event.key.col, record->event.key.row);
				}
				break;
		}
		return false; // press AND release both forwarded, so tutorial.c can track hold-state
	}

	/*** Any other full-screen mode (menu pages, Breakout, ...): the buttons do
		whatever ui_button_actions[] says on press, and no keycode ever reaches
		the host while it's up.
	***/
	if (mode != UI_MODE_IDLE) {
		if (record->event.pressed) {
			const ui_button_actions_t *actions = &ui_button_actions[mode];
			if (keycode == KC_BUTTON_1 && actions->button1) {
				actions->button1();
			} else if (keycode == KC_BUTTON_2 && actions->button2) {
				actions->button2();
			}
		}
		return false;
	}

	/*** Idle screen
		+ Button 1 -> previous layer on release (its normal tap action), unless
		  it was already held long enough to auto-trigger the "DIAL SETTINGS"
		  sub-page - see housekeeping_task_display()
		+ Pressing Button 2 -> next layer (acts on press, like always)
	***/
	if (record->event.pressed) {
		switch (keycode) {
			case KC_BUTTON_1:
				button1_held       = true;
				button1_press_time = timer_read32();
				return false;
			case KC_BUTTON_2:
				if (get_highest_layer(layer_state) >= DYNAMIC_KEYMAP_LAYER_COUNT-1)
					layer_move(0);
				else
					layer_move(get_highest_layer(layer_state)+1);
				return false;
			default:
				break; // Process all other keycodes normally
		}
	} else if (keycode == KC_BUTTON_1) {
		if (button1_held) { // false if the hold already fired dial_menu_open() and left the idle screen
			button1_held = false;
			if (get_highest_layer(layer_state) <= 0)
				layer_move(DYNAMIC_KEYMAP_LAYER_COUNT-1);
			else
				layer_move(get_highest_layer(layer_state)-1);
		}
		return false;
	}

	widget_matrix_update(record->event.key.col, record->event.key.row);
	return true;
}
