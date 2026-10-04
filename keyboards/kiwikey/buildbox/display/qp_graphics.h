#pragma once

#include "quantum.h"
#include <qp.h>

#include "display/ui_mode.h"     // ui_get_mode()/ui_set_mode()/ui_idle_screen_visible()
#include "features/webhid_shadow.h" // redirects qp_rect/qp_flush/etc to also mirror over webhid_stream; see that header

#define BOOT_DURATION          4000 // ms

// eepdata.display_timeout is an INDEX into these two, not a raw seconds value -
// cycled in menu_process_rotation() (qp_menu.c) via MENU_DISPLAYTIMEOUT,
// same pattern as eepdata.knob_func/knob_effect/screensaver_effect.
#define DISPLAY_TIMEOUT_COUNT        6
#define DISPLAY_TIMEOUT_NEVER_INDEX  (DISPLAY_TIMEOUT_COUNT - 1) // last entry = "NEVER"
#define DISPLAY_TIMEOUT_1HOUR_INDEX  (DISPLAY_TIMEOUT_COUNT - 2) // second-to-last entry = "1 Hour" - also arms the screensaver, see qp_widget_screensaver.c

extern const uint32_t display_timeout_seconds[DISPLAY_TIMEOUT_COUNT];
extern const char * const display_timeout_text[DISPLAY_TIMEOUT_COUNT];

extern painter_device_t bb_display;
extern bool display_ready; // false until display_init() has created the device and loaded all fonts/images

// Redraw requests raised from VIA (features/via_custom.c), consumed in housekeeping_task_display()
extern uint16_t flag_display_keycode_changed;
extern bool     flag_display_keymap_reload;
extern uint8_t  flag_widget_layer_changed;

void display_init(void);
void ui_refresh(void);

void keyboard_post_init_display(void);
void housekeeping_task_display(void);
bool process_record_display(uint16_t keycode, keyrecord_t *record);
bool display_is_asleep(void); // true while the backlight is zeroed for the idle timeout - see housekeeping_task_display()

