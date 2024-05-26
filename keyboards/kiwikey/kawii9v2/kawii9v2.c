// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "oled_key_matrix.h"
#include "oled_qmk_logo.h"
#include "oled_wpm_graph.h"
#include "oled_ui.h"
#include "oled_menu.h"

uint32_t key_timer = 0;

void keyboard_post_init_kb(void) {
    render_ui_frame();
    keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (menu_is_enabled && record->event.pressed) {
        process_record_menu(keycode, record);
        return false;
    }
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
        case MENU_ENABLE_KEYCODE:
            if (!menu_is_enabled) {
				menu_is_enabled = true;
				menu_init();
			}
        default:
            break;
    }
	
#ifdef OLED_KEY_MATRIX
    render_matrix();
#endif // OLED_KEY_MATRIX

    key_timer = timer_read32();
    return process_record_user(keycode, record);
}
