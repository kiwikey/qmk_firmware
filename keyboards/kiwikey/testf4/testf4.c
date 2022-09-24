/* Copyright 2022 KiwiKey
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

#include "testf4.h"

#include <qp.h>

static painter_device_t display;

void keyboard_post_init_kb(void) {
    display = qp_st7789_make_spi_device(240, 240, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, DISPLAY_SPI_DIVISOR, 3);   // Create the display
    qp_init(display, QP_ROTATION_0);   // Initialise the display
	//qp_clear(display);
	//qp_line(display, 0, 5, 7, i, i, 255, 255);
	//qp_rect(display, 0, 7, 100, 239, 255, 255, 255, true);
	//qp_flush(display);
}

void housekeeping_task_kb(void) {
    static uint32_t last_draw = 0;
    if (timer_elapsed32(last_draw) > 33) { // Throttle to 30fps
        last_draw = timer_read32();
        // Draw 8px-wide rainbow down the left side of the display
        for (int i = 0; i < 239; ++i) {
            qp_line(display, 0, i, 7, i, i, 255, 255);
        }
        qp_flush(display);
    }
}


/*
void ui_init(void) {
	qp_display = qp_st7789_make_spi_device(240, 240, B9, B8, B7, DISPLAY_SPI_DIVISOR, 3);
	qp_init(qp_display, QP_ROTATION_0);
	qp_clear(qp_display);
    wait_ms(50);
    qp_power(qp_display, true);
    // also, passing in hsv is kinda dumb, should be a colour
    qp_rect(qp_display, 0, 0, 240, 106, HSV_BLACK, true);
    qp_rect(qp_display, 0, 106, 240, 212, HSV_BLACK, true);
    qp_rect(qp_display, 0, 212, 220, 240, HSV_BLACK, true);
    qp_flush(qp_display);
}
*/
