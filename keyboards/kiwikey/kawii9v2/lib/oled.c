// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later
 
#include "quantum.h"
#include "oled_key_matrix.h"
#include "oled_qmk_logo.h"
#include "oled_wpm_graph.h"
#include "oled_ui.h"
#include "oled_menu.h"

#ifdef OLED_ENABLE

extern uint32_t key_timer;

bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }
    if (menu_is_enabled) { // while in Menu, skip all other OLED processes
        return false;
    }
    render_stats();
    switch (sub_ui_mode) {
        case 0:
            render_wpm_graph();
            render_qmk_logo(0, 5);
            break;
        case 1:
            render_ui_rgbcontrol();
            break;
    }
    if ((sub_ui_mode != 0) && (timer_elapsed32(key_timer) > SUB_UI_TIMEOUT)) { // If timeout, close sub UI & back to default UI
        sub_ui_mode = 0;
        sub_ui_clear();
    }
    if (timer_elapsed32(key_timer) > OLED_TIMEOUT) {
        oled_off();
    }
    return false;
}

bool shutdown_kb(bool jump_to_bootloader) {
    oled_clear();
	oled_set_cursor(4, 3);
	if (jump_to_bootloader) {
		oled_write_P(PSTR("> DFU Mode <"), false);
	} else {
		// oled_write_P(PSTR("Rebooting "), false);
	}
    oled_render_dirty(true);
	return true;
}

#endif // defined(OLED_ENABLE)
