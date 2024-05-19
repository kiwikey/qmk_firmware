// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later
 
#include "quantum.h"
#include "oled_custom_api.h"
#include "oled_bongocat_rle.c"

#ifdef OLED_ENABLE

static uint32_t key_timer = 0;
static uint8_t rgb_val = 0;

// Sub UI is the bottom half of OLED screen, shows WPM chart, RGB infos,...
// sub_ui_mode = 0 : WPM graph
//               1 : RGB control
//               2 : Media control (to do)
#define SUB_UI_DISPLAY_ROW 4
#define SUB_UI_TIMEOUT 2000 // ms
static char sub_ui_mode = 0;

// Configuration for Stats (by COL/ROW count, not by pixel)
#define STATS_DISPLAY_COL 0
#define STATS_DISPLAY_ROW 1 // Be careful setting this to 0 (zero)
/*** Example of Stats ***
    LAYER 0  WPM 123
    
    RGB #03  * 55%
************************/

// Configuration for Interactive Key Matrix (by pixel)
#define MATRIX_DISPLAY_X    107
#define MATRIX_DISPLAY_Y    STATS_DISPLAY_ROW * 8 + 2 // The Matrix follows the Stats (same height)
#define MATRIX_DISPLAY_SIZE 22
#define KEY_SIZE            4
#define KEY_SPACING         3

// Configuration for WPM Graph
// #define GRAPH_ZERO_X           20  // This is zero origin
// #define GRAPH_ZERO_Y           63  // of the WPM graph
// #define GRAPH_WIDTH            105
// #define GRAPH_HEIGHT           28
// #define GRAPH_REFRESH_INTERVAL 100 // ms
// #define GRAPH_LINE_THICKNESS   1
// float max_wpm = 100.0;
// uint16_t graph_timer = 0;

// RGB Matrix naming, copy from @tzarc
#if defined(RGB_MATRIX_ENABLE)
#include <rgb_matrix.h>

#if defined(RGB_MATRIX_EFFECT)
#    undef RGB_MATRIX_EFFECT
#endif // defined(RGB_MATRIX_EFFECT)
#define RGB_MATRIX_EFFECT(x) RGB_MATRIX_EFFECT_##x,

enum {
    RGB_MATRIX_EFFECT_NONE,
#include "rgb_matrix_effects.inc"
#undef RGB_MATRIX_EFFECT
#ifdef RGB_MATRIX_CUSTOM_KB
#    include "rgb_matrix_kb.inc"
#endif
#ifdef RGB_MATRIX_CUSTOM_USER
#    include "rgb_matrix_user.inc"
#endif
};

#define RGB_MATRIX_EFFECT(x)    \
    case RGB_MATRIX_EFFECT_##x: \
        return #x;
const char* rgb_matrix_name(uint8_t effect) {
    switch (effect) {
        case RGB_MATRIX_EFFECT_NONE:
            return "NONE";
#include "rgb_matrix_effects.inc"
#undef RGB_MATRIX_EFFECT
#ifdef RGB_MATRIX_CUSTOM_KB
#    include "rgb_matrix_kb.inc"
#endif
#ifdef RGB_MATRIX_CUSTOM_USER
#    include "rgb_matrix_user.inc"
#endif
        default:
            return "UNKNOWN";
    }
}
#endif // defined(RGB_MATRIX_ENABLE)

void render_matrix(void) {
    for (uint8_t x = 0; x < MATRIX_ROWS-2; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS-2; y++) {
            bool on = (matrix_get_row(x) & (1 << y)) > 0; // The matrix position [x,y] is being pressed
            draw_rect( MATRIX_DISPLAY_X + y*(KEY_SIZE+KEY_SPACING), // X - horizontal position of key
                       MATRIX_DISPLAY_Y + x*(KEY_SIZE+KEY_SPACING), // Y - vertical position of key
                       KEY_SIZE,
                       KEY_SIZE,
                       on);
        }
    }
}

void ui_clear(void) {
    oled_set_cursor(0, 4);
    oled_advance_page(true);
    oled_advance_page(true);
    oled_advance_page(true);
    oled_advance_page(true);
}

void render_ui_frame(void) {
    oled_clear();
    oled_set_cursor(STATS_DISPLAY_COL, STATS_DISPLAY_ROW);
    oled_write_P(PSTR("LAYER    WPM"), false);
    oled_set_cursor(STATS_DISPLAY_COL, STATS_DISPLAY_ROW+2);
    oled_write_P(PSTR("RGB"), false);
    // Outline for layer number
    // draw_line_h(35, 7, 7, true);
    // draw_line_v(34, 8, 7, true);
    // draw_line_v(35, 8, 8, true);
    // draw_line_v(42, 8, 7, true);
	if (STATS_DISPLAY_ROW) // If STATS_DISPLAY_ROW = 0, the horizontal line is out of screen, better remove it to avoid unexpected bug
		draw_line_h((STATS_DISPLAY_COL+6)*6-1, STATS_DISPLAY_ROW*8-1, 7, true);
	draw_line_v((STATS_DISPLAY_COL+6)*6-2, STATS_DISPLAY_ROW*8,   7, true);
    draw_line_v((STATS_DISPLAY_COL+6)*6-1, STATS_DISPLAY_ROW*8,   8, true);
    draw_line_v((STATS_DISPLAY_COL+6)*6+6, STATS_DISPLAY_ROW*8,   7, true);
    // Outline for RGB mode number
    // draw_line_h(23, 23, 19, true);
    // draw_line_v(22, 24, 7, true);
    // draw_line_v(23, 24, 8, true);
    // draw_line_v(42, 24, 7, true);
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
    oled_set_cursor(0, 6);
    oled_write_ln(rgb_matrix_name(rgb_matrix_get_mode()), false);
    // RGB brightness bar
    oled_set_cursor(0, 7);
    oled_write_char((rgb_val == 0) ? 0x07 : 0x9D, false);
    for (uint8_t i = 0; i < 20; i++) {
        oled_write_char(0x20, i*5 < rgb_val);
    }
}

void render_stats(void) {
    // Current layer
    oled_set_cursor(STATS_DISPLAY_COL+6, STATS_DISPLAY_ROW);
    oled_write_char(get_highest_layer(layer_state) + 0x30, true);
    // Current WPM
    oled_set_cursor(STATS_DISPLAY_COL+13, STATS_DISPLAY_ROW);
    oled_write(get_u8_str(get_current_wpm(), '0'), false);
    // Show current RGB mode (#__)
    oled_set_cursor(STATS_DISPLAY_COL+4, STATS_DISPLAY_ROW+2);
    if (rgb_matrix_is_enabled()) {
        oled_write_P(PSTR("#"), true);
        oled_write_char(rgb_matrix_get_mode()/10 + 0x30, true);
        oled_write_char(rgb_matrix_get_mode()%10 + 0x30, true);
    }
    else {
        oled_write_P(PSTR("OFF"), true);
    }
    // RGB brightness (percentage %)
    oled_set_cursor(STATS_DISPLAY_COL+9, STATS_DISPLAY_ROW+2);
    rgb_val = (rgb_matrix_is_enabled() ? rgb_matrix_get_val()*100/RGB_MATRIX_MAXIMUM_BRIGHTNESS : 0);
    if (rgb_matrix_is_enabled()) {
        oled_write_char((rgb_val == 0) ? 0x07 : 0x9D, false);
        oled_write(get_u8_str(rgb_val, ' '), false);
        oled_write_char(0x25, false); // %
    }
    else {
        oled_write_P(PSTR("-----"), false);
    }
	// uint32_t via_layout = via_get_layout_options();
	// oled_set_cursor(0,0);
	// oled_write_P(PSTR("VIA Layout: "), false);
	// oled_write(get_u8_str(via_layout, ''), false);	
}

void keyboard_post_init_kb(void) {
    oled_on();
    oled_init(OLED_ROTATION_0);
    render_ui_frame();
}

bool oled_task_kb(void) {
    render_stats();
	render_bongocat();
    switch (sub_ui_mode) {
        case 0:
            //render_wpm_graph();
            //render_anim();
            break;
        case 1:
            render_ui_rgbcontrol();
            break;
    }
    if ((sub_ui_mode != 0) && (timer_elapsed32(key_timer) > SUB_UI_TIMEOUT)) { // If timeout, back to default UI
        sub_ui_mode = 0;
        ui_clear();
    }
    if (timer_elapsed32(key_timer) > OLED_TIMEOUT) {
        oled_off();
    }
    return false;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    // oled_on();
	
    switch (keycode) {
        case QK_LIGHTING ... QK_LIGHTING_MAX:
            sub_ui_mode = 1;
            ui_clear();
            break;
        // case QK_BOOT: // when activating RESET mode for flashing
            // if (record->event.pressed) {
				// oled_clear();
				// oled_write_P(PSTR("Bootloader Mode"), false);
            // }
            // return true;
        default:
            break;
    }
	
	if (record->event.pressed) {
		extern uint32_t oled_tap_timer;
		oled_tap_timer = timer_read32();
	}
    render_matrix();
    key_timer = timer_read32();
    return process_record_user(keycode, record);
}

#endif // defined(OLED_ENABLE)
