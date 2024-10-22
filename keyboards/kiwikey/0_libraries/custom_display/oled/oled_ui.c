#if defined(OLED_ENABLE)

#include "oled_ui.h"
#include "encoder.h"
#include "eeprom_custom.h"
#include "oled_custom_api.h"
#include "rgb_matrix_name.h"
#include "oled_key_matrix.h"

uint8_t rgb_val;
uint8_t sub_ui_mode = 0;

void ui_refresh(void) {
	oled_clear();
	render_ui_frame();
	render_stats();
}

void render_ui_frame(void) { // Sometimes while coding, magical things happen, just copy do not ask <(")
    oled_clear();
    oled_set_cursor(STATS_DISPLAY_COL, STATS_DISPLAY_ROW);
    oled_write_P(PSTR("LAYER"), false);
    oled_set_cursor(STATS_DISPLAY_COL, STATS_DISPLAY_ROW +2);
    oled_write_P(PSTR("RGB"), false);
    // Outline for layer number
	if (STATS_DISPLAY_ROW) // If STATS_DISPLAY_ROW == 0, the horizontal line is out of screen, better remove it to avoid unexpected bug
		draw_line_h((STATS_DISPLAY_COL+6)*6-1, STATS_DISPLAY_ROW*8-1, 43, true);
	draw_line_v((STATS_DISPLAY_COL+6)*6-2,  STATS_DISPLAY_ROW*8,   7, true);
    draw_line_v((STATS_DISPLAY_COL+6)*6-1,  STATS_DISPLAY_ROW*8,   8, true);
    draw_line_v((STATS_DISPLAY_COL+6)*6+42, STATS_DISPLAY_ROW*8,   7, true);
    // Outline for RGB mode number
    draw_line_h((STATS_DISPLAY_COL+4)*6-1, (STATS_DISPLAY_ROW+2)*8-1, 19, true);
	draw_line_v((STATS_DISPLAY_COL+4)*6-2, (STATS_DISPLAY_ROW+2)*8,   7, true);
    draw_line_v((STATS_DISPLAY_COL+4)*6-1, (STATS_DISPLAY_ROW+2)*8,   8, true);
    draw_line_v((STATS_DISPLAY_COL+6)*6+6, (STATS_DISPLAY_ROW+2)*8,   7, true);
    // Outline for Key Matrix
    draw_line_h(MATRIX_DISPLAY_X -2, MATRIX_DISPLAY_Y -3, MATRIX_DISPLAY_SIZE, true);
    draw_line_h(MATRIX_DISPLAY_X -2, MATRIX_DISPLAY_Y -2 + MATRIX_DISPLAY_SIZE, MATRIX_DISPLAY_SIZE, true);
    draw_line_v(MATRIX_DISPLAY_X -3, MATRIX_DISPLAY_Y -2, MATRIX_DISPLAY_SIZE, true);
    draw_line_v(MATRIX_DISPLAY_X -2 + MATRIX_DISPLAY_SIZE, MATRIX_DISPLAY_Y-2, MATRIX_DISPLAY_SIZE, true);
}

void sub_ui_clear(void) { // from SUB_UI_DISPLAY_ROW to last OLED line
    oled_set_cursor(0, SUB_UI_DISPLAY_ROW);
	for (uint8_t i = 0; i < oled_max_lines()-SUB_UI_DISPLAY_ROW; i++) {
		oled_advance_page(true);
	}
	if (sub_ui_mode) // If a sub UI is activated, draw the horizontal line for its title
		draw_line_h(0, 43, 126, true);
}

void render_subui_rgbcontrol(void) {
    oled_set_cursor(6, SUB_UI_DISPLAY_ROW +1);
    oled_write_P(PSTR(" Lighting "), false);
	
#ifdef RGB_MATRIX_ENABLE
    // RGB mode
    oled_set_cursor(0, SUB_UI_DISPLAY_ROW +2);
	oled_write_ln(rgb_matrix_is_enabled() ? rgb_matrix_name(rgb_matrix_get_mode()) : "RGB_OFF", false);
	//oled_write_ln(rgbmode_names[rgb_matrix_get_mode()], false); // Need to enable rgbmode_names[] in 'rgb_matrix_name.c'

    // RGB brightness bar
    oled_set_cursor(0, SUB_UI_DISPLAY_ROW +3);
    oled_write_char((rgb_val == 0) ? 0x07 : 0x9D, false);
    for (uint8_t i = 0; i < 20; i++) {
        oled_write_char(0x20, i*5 < rgb_val);
    }
#endif // RGB_MATRIX_ENABLE
}

void render_stats(void) {
    // Current layer
    oled_set_cursor(STATS_DISPLAY_COL +6, STATS_DISPLAY_ROW);
	// oled_write_char(get_highest_layer(layer_state) + 0x30, true);
	oled_write(layer_name[get_highest_layer(layer_state)], true);
	
#ifdef WPM_ENABLE
    // Current WPM
    oled_set_cursor(STATS_DISPLAY_COL +13, STATS_DISPLAY_ROW);
    oled_write(get_u8_str(get_current_wpm(), '0'), false);
#endif // WPM_ENABLE

#ifdef RGB_MATRIX_ENABLE
    // Show current RGB mode (#__)
    oled_set_cursor(STATS_DISPLAY_COL +4,STATS_DISPLAY_ROW +2);
    if (rgb_matrix_is_enabled()) {
        // oled_write_P(PSTR("#"), true);
		oled_write_char(0x23, true); // try to save 2 bytes, omg I am so poor
        oled_write_char(rgb_matrix_get_mode()/10 + 0x30, true);
        oled_write_char(rgb_matrix_get_mode()%10 + 0x30, true);
    }
    else {
        oled_write_P(PSTR("OFF"), true);
    }
    // RGB brightness (percentage %)
    oled_set_cursor(STATS_DISPLAY_COL +8, STATS_DISPLAY_ROW +2);
    rgb_val = (rgb_matrix_is_enabled() ? rgb_matrix_get_val()*100/RGB_MATRIX_MAXIMUM_BRIGHTNESS : 0);
    if (rgb_matrix_is_enabled()) {
        oled_write_char((rgb_val == 0) ? 0x07 : 0x9D, false);
        oled_write(get_u8_str(rgb_val, ' '), false);
        oled_write_char(0x25, false); // %
    }
    else {
		oled_write_char(0x07, false);
        oled_write_P(PSTR("  0%"), false);
    }
	// Knob function
	oled_set_cursor(19,1); oled_write_char(0x5E, false); // Arrow up
	oled_set_cursor(19,3); oled_write_char(0xDD, false); // Arrow up
	oled_set_cursor(18,2); oled_write(encoder_func_shortname[eepdata.knob_func],false);
	
	#endif // RGB_MATRIX_ENABLE
}

#endif // defined(OLED_ENABLE)
