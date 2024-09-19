// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later
 
#include "oled.h"

#include "eeprom_custom.h"
#include "encoder.h"
#include "oled/oled_key_matrix.h"
#include "oled/oled_ui.h"
#include "oled/oled_menu.h"
#include "oled/img/anim_qmk_logo.c"
#include "oled/img/anim_bongocat_rle.c"
#include "oled/img/anim_mario.c"

bool oled_task_kb(void) {
	// oled_set_cursor(0,0);
    // oled_write_P(PSTR("K="), false);
	// oled_write_char(eepdata.knob_func+0x30, false);
    // oled_write_P(PSTR(" S="), false);
	// oled_write_char(VIA_EEPROM_CUSTOM_CONFIG_SIZE/10+0x30, false);
	// oled_write_char(VIA_EEPROM_CUSTOM_CONFIG_SIZE%10+0x30, false);
	// oled_write_P(PSTR(" Sub: "), false);
	// oled_write_char(oled_max_lines()-SUB_UI_DISPLAY_ROW+0x30, false);

    if (!oled_task_user()) {
        return false;
    }
	
	/*** Manually turn off OLED, if OLED timeout reached & not "Always ON" (even in Menu) ***/
	if (is_oled_on()) {
		if ((eepdata.display_timeout != DISPLAY_TIMEOUT_NEVER) && ((last_input_activity_elapsed()/1000) > eepdata.display_timeout)) {
			if (current_menu != NOT_IN_MENU) menu_exit();
			oled_off();
		}
    } else if ((last_input_activity_elapsed()/1000) < eepdata.display_timeout) { // Manually turn on OLED, encoder's input does not do this (not sure why)
		oled_on();
		ui_refresh();
	}
	
	/*** Turn off Sub UI if timeout ***/
	if ((sub_ui_mode != 0) && (last_input_activity_elapsed() > SUB_UI_TIMEOUT)) {
        sub_ui_mode = 0;
        if (current_menu == NOT_IN_MENU)
			sub_ui_clear();
    }
	
	/*** If in Menu, skip all other OLED processes ***/
    if (current_menu != NOT_IN_MENU) {
		if ((last_input_activity_elapsed()/1000) > DISPLAY_TIMEOUT_MIN) // Turn off Menu automatically after DISPLAY_TIMEOUT_MIN
			menu_exit();
        return false;
    }
	
	/*** If not in Menu, render other widgets ***/
    render_stats();
    switch (sub_ui_mode) {
        case 0: // Sub-UI is not running -> render animation
			#if defined(WPM_ENABLE)
            render_wpm_graph();
			#endif // defined(WPM_ENABLE)
            switch (eepdata.display_anim) {
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
        case 1: // Sub-UI #1
            render_subui_rgbcontrol();
            break;
		default:
			break;
    }

    return false;
}

bool process_record_oled(uint16_t keycode, keyrecord_t *record) {

	if (current_menu != NOT_IN_MENU) { // If being in MENU, only process the encoder, skip all other keycodes
		if (record->event.key.col != 3) // If not encoder press, leave here (only 1 condition is needed)
			return false;
    }
	
	# ifdef OLED_KEY_MATRIX
	if (record->event.key.col != 3) // TODO: hotfix, do not render "key matrix" if the encoder is pressed
		render_matrix();
	# endif // OLED_KEY_MATRIX

    switch (keycode) {
        case QK_LIGHTING ... QK_LIGHTING_MAX:
            sub_ui_mode = 1;
            sub_ui_clear();
            break;
        /*** TODO
        case KC_AUDIO_MUTE ... KC_MEDIA_EJECT:
            sub_ui_mode = 2;
            sub_ui_clear();
            break;
        ***/
        default:
			// if ((sub_ui_mode != 0) && record->event.pressed) { // any non-related keycode will turn off sub UI
			    // sub_ui_mode = 0;
				// sub_ui_clear();
			// }
            break;
    }
	
	return true;
}
