// Sub UI is the bottom half of OLED screen, shows WPM chart, RGB infos,...
// sub_ui_mode = 0 : WPM graph
//               1 : RGB control
//               2 : Media control (to do)
#define SUB_UI_DISPLAY_ROW 4
#define SUB_UI_TIMEOUT 2000 // ms
static char sub_ui_mode = 0;

// Display configuration for stats (by COL/ROW count, not by pixel)
#define STATS_DISPLAY_COL 0
#define STATS_DISPLAY_ROW 1 // Be careful setting this to 0 (zero)
/*** Example of Stats display ***
    LAYER 0  WPM 123
    
    RGB #03  * 55%
********************************/

extern uint8_t rgb_val;

void sub_ui_clear(void) {
    oled_set_cursor(0, SUB_UI_DISPLAY_ROW);
    oled_advance_page(true);
    oled_advance_page(true);
    oled_advance_page(true);
    oled_advance_page(true);
}

/********** OLD & not-flexible UI frame **********
void render_ui_frame(void) {
    oled_clear();
    oled_advance_page(false);
    oled_write_ln_P(PSTR("LAYER    WPM"), false);
    oled_advance_page(false);
    oled_write_ln_P(PSTR("RGB"), false);
    oled_advance_page(false);
    oled_advance_page(false);
    oled_advance_page(false);
    // Outline for layer number
    draw_line_h(35, 7, 7, true);
    draw_line_v(34, 8, 7, true);
    draw_line_v(35, 8, 8, true);
    draw_line_v(42, 8, 7, true);
    // Outline for RGB mode number
    draw_line_h(23, 23, 19, true);
    draw_line_v(22, 24, 7, true);
    draw_line_v(23, 24, 8, true);
    draw_line_v(42, 24, 7, true);
    // Outline for Key Matrix
    draw_line_h(MATRIX_DISPLAY_X -2, MATRIX_DISPLAY_Y -3, MATRIX_DISPLAY_SIZE, true);
    draw_line_h(MATRIX_DISPLAY_X -2, MATRIX_DISPLAY_Y -2 + MATRIX_DISPLAY_SIZE, MATRIX_DISPLAY_SIZE, true);
    draw_line_v(MATRIX_DISPLAY_X -3, MATRIX_DISPLAY_Y -2, MATRIX_DISPLAY_SIZE, true);
    draw_line_v(MATRIX_DISPLAY_X -2 + MATRIX_DISPLAY_SIZE, MATRIX_DISPLAY_Y-2, MATRIX_DISPLAY_SIZE, true);
}
***********************************************/

void render_ui_frame(void) { // Sometimes while coding, magical things happen, just copy do not ask <(")
    oled_clear();
    oled_set_cursor(STATS_DISPLAY_COL, STATS_DISPLAY_ROW);
    oled_write_P(PSTR("LAYER    WPM"), false);
    oled_set_cursor(STATS_DISPLAY_COL, STATS_DISPLAY_ROW +2);
    oled_write_P(PSTR("RGB"), false);
    // Outline for layer number
	if (STATS_DISPLAY_ROW) // If STATS_DISPLAY_ROW = 0, the horizontal line is out of screen, better remove it to avoid unexpected bug
		draw_line_h((STATS_DISPLAY_COL+6)*6-1, STATS_DISPLAY_ROW*8-1, 7, true);
	draw_line_v((STATS_DISPLAY_COL+6)*6-2, STATS_DISPLAY_ROW*8,   7, true);
    draw_line_v((STATS_DISPLAY_COL+6)*6-1, STATS_DISPLAY_ROW*8,   8, true);
    draw_line_v((STATS_DISPLAY_COL+6)*6+6, STATS_DISPLAY_ROW*8,   7, true);
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

void render_ui_rgbcontrol(void) {
    oled_set_cursor(0, SUB_UI_DISPLAY_ROW);
	oled_advance_page(true);
    oled_write_P(PSTR("----< Lighting >-----"), false);
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
}

void render_stats(void) {
    // Current layer
    oled_set_cursor(STATS_DISPLAY_COL +6, STATS_DISPLAY_ROW);
    oled_write_char(get_highest_layer(layer_state) + 0x30, true);
    // Current WPM
    oled_set_cursor(STATS_DISPLAY_COL +13, STATS_DISPLAY_ROW);
    oled_write(get_u8_str(get_current_wpm(), '0'), false);
    // Show current RGB mode (#__)
    oled_set_cursor(STATS_DISPLAY_COL +4,STATS_DISPLAY_ROW +2);
    if (rgb_matrix_is_enabled()) {
        oled_write_P(PSTR("#"), true);
        oled_write_char(rgb_matrix_get_mode()/10 + 0x30, true);
        oled_write_char(rgb_matrix_get_mode()%10 + 0x30, true);
    }
    else {
        oled_write_P(PSTR("OFF"), true);
    }
    // RGB brightness (percentage %)
    oled_set_cursor(STATS_DISPLAY_COL +9, STATS_DISPLAY_ROW +2);
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
}
