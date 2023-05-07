/* Copyright 2022 DZTECH <moyi4681@live.cn>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "quantum.h"
#include "lib/bongocat.h"
#include "eeprom_driver.h"
#include "via.h"

#ifdef OLED_ENABLE

#define EEPROM_OLED_MODE (VIA_EEPROM_CUSTOM_CONFIG_ADDR)
static char oled_status_flag = 0;
// 0: BongoCat
// 1: Status + BongoCat
// 2: OLED off


#define GRAPH_ZERO_X           27   // This is zero origin
#define GRAPH_ZERO_Y           63  // of the graph
#define GRAPH_WIDTH            97
#define GRAPH_HEIGHT           23
#define GRAPH_REFRESH_INTERVAL 100 // ms
#define GRAPH_LINE_THICKNESS   1
float max_wpm = 120.0;
uint32_t timer = 0;

void draw_line_h(uint8_t x, uint8_t y, uint8_t len, bool on) {
    for (uint8_t i = 0; i < len; i++) {
        oled_write_pixel(i + x, y, on);
    }
}
void draw_line_v(uint8_t x, uint8_t y, uint8_t len, bool on) {
    for (uint8_t i = 0; i < len; i++) {
        oled_write_pixel(x, i + y, on);
    }
}
void draw_rect(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, bool on) {
    for (uint8_t i = 0; i < sizey; i++) {
        draw_line_h(x, y+i, sizex, on);
    }
}

void draw_wpm_graph(void) {
    uint8_t curr_wpm = (get_current_wpm() < max_wpm ? get_current_wpm() : max_wpm);
    if (timer_elapsed(timer) > GRAPH_REFRESH_INTERVAL) {
        uint8_t graph_pointer = (curr_wpm / max_wpm) * GRAPH_HEIGHT;
        draw_line_h(GRAPH_ZERO_X + 1, GRAPH_ZERO_Y - graph_pointer, GRAPH_LINE_THICKNESS, true);
        for (char i = 0; i < graph_pointer; i++) {
            draw_line_h(GRAPH_ZERO_X + 1, GRAPH_ZERO_Y - i, GRAPH_LINE_THICKNESS, true);
        }
        for (char i = 0; i < GRAPH_LINE_THICKNESS; i++) {
            oled_pan_area(GRAPH_ZERO_X,
                          GRAPH_ZERO_X + GRAPH_WIDTH,
                          (GRAPH_ZERO_Y + 1 - GRAPH_HEIGHT) / 8,
                          (GRAPH_ZERO_Y + 1) / 8 - 1,
                          false);
        }
        timer = timer_read();
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    //return OLED_ROTATION_180;
    return OLED_ROTATION_0;
}

void keyboard_post_init_user(void) {
	switch (eeprom_read_byte((uint8_t*)EEPROM_OLED_MODE)) {
		case 0x00:
			oled_status_flag = 0;
			break;
		case 0x01:
			oled_status_flag = 1;
			break;
		case 0x02:
			oled_status_flag = 2;
			break;
	}
}

bool oled_task_kb(void) {
	if (!oled_task_user()) { return false; }
	led_t led_usb_state = host_keyboard_led_state();
	
	// sWiTcH - CaSe iS fOr N00b lol maybe later
	if (oled_status_flag == 0) {
		oled_set_cursor(0, 2);
		render_bongocat();
	}
    else if (oled_status_flag == 1) {
		draw_wpm_graph();
		render_bongocat();
		oled_set_cursor(14, 5); // sets cursor to (column, row) using charactar spacing (4 rows on 128x32 screen, anything more will overflow back to the top)
		oled_write_P(PSTR("WPM:"), false);
		oled_write(get_u8_str(get_current_wpm(), '0'), false); // writes wpm on top right corner of string
		oled_set_cursor(0, 5);
		oled_write_P(led_usb_state.caps_lock ? PSTR("CAPS") : PSTR("    "), false);
		oled_set_cursor(0, 6);
		oled_write_P(led_usb_state.num_lock ? PSTR("NUM") : PSTR("    "), false);
		oled_set_cursor(0, 7);
		oled_write_P(led_usb_state.scroll_lock ? PSTR("SCRL") : PSTR("    "), false);
		oled_set_cursor(0, 5);
	}
	else if (oled_status_flag == 2) {
		oled_off();
	}
	return true;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	if (keycode == KC_F9) {
		oled_clear();
		oled_status_flag = (oled_status_flag == 2 ? 0 : oled_status_flag + 1);
		eeprom_write_byte((uint8_t*)EEPROM_OLED_MODE, oled_status_flag);
    }
	return true;
}

#endif
