// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later
 
#include "quantum.h"
#include "oled_custom_api.h"
#include "rgb_matrix_name.c"
#include "oled_key_matrix.c"
#include "oled_qmk_logo.c"
#include "oled_ui.c"
#include "oled_wpm_graph.c"

#ifdef OLED_ENABLE

uint32_t key_timer = 0;
uint8_t rgb_val = 0;

void keyboard_post_init_kb(void) {
    render_ui_frame();
    keyboard_post_init_user();
}

bool oled_task_kb(void) {
    if (!oled_task_user()) {
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
    if ((sub_ui_mode != 0) && (timer_elapsed32(key_timer) > SUB_UI_TIMEOUT)) { // If timeout, back to default UI
        sub_ui_mode = 0;
        sub_ui_clear();
    }
    if (timer_elapsed32(key_timer) > OLED_TIMEOUT) {
        oled_off();
    }
    return false;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_LIGHTING ... QK_LIGHTING_MAX:
            sub_ui_mode = 1;
            sub_ui_clear();
            break;
        /** TODO 
        case KC_AUDIO_MUTE ... KC_MEDIA_EJECT:
            sub_ui_mode = 2;
            sub_ui_clear();
            break;
        ***/
        default:
            break;
    }
    render_matrix();
    key_timer = timer_read32();
    return process_record_user(keycode, record);
}

#endif // defined(OLED_ENABLE)
