// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later
 
#include "quantum.h"

#if defined(OLED_ENABLE)

#include "oled_key_matrix.h"
#include "oled_wpm_graph.h"
#include "oled_ui.h"
#include "oled_menu.h"
#include "anim_qmk_logo.c"
#include "anim_bongocat_rle.c"
#include "anim_mario.c"

extern uint32_t key_timer;
extern uint8_t  eepdata_active_layer,
                eepdata_oled_anim,
                eepdata_oled_timeout;

bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }
    if (current_menu != NOT_IN_MENU) { // while in Menu, skip all other OLED processes
        return false;
    }
    render_stats();
    switch (sub_ui_mode) {
        case 0:
#if defined(WPM_ENABLE)
            render_wpm_graph();
#endif // defined(WPM_ENABLE)
            switch (eepdata_oled_anim) {
                case 0:
                    break;
                case 1:
                    render_qmk_logofull(1, 5);
                    break;
                case 2:
                    render_anim();
                    break;
                case 3:
                    render_bongocat();
                    break;
            }
            break;
        case 1:
            render_ui_rgbcontrol();
            break;
    }
    if ((sub_ui_mode != 0) && (timer_elapsed32(key_timer) > SUB_UI_TIMEOUT)) { // If timeout, close sub UI & back to default UI
        sub_ui_mode = 0;
        sub_ui_clear();
    }
    if ((eepdata_oled_timeout != OLED_TIMEOUT_NEVER) && ((timer_elapsed32(key_timer)/1000) > eepdata_oled_timeout)) {
        oled_off();
    }
    return false;
}

#endif // defined(OLED_ENABLE)
