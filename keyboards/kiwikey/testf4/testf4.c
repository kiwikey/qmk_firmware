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
#include "print.h"
#include "spi_master.h"

//static painter_device_t display;
painter_device_t display;

void keyboard_post_init_kb(void) {
	wait_ms(1000);
	
    display = qp_st7789_make_spi_device(240, 240, DISPLAY_CS_PIN, DISPLAY_DC_PIN, DISPLAY_RST_PIN, 8, 3);   // Create the display
	wait_ms(100);
    qp_init(display, QP_ROTATION_0);   // Initialise the display
	wait_ms(100);
	
	// Turn on the LCD and clear the display
	qp_power(display, true);wait_ms(100);
    qp_rect(display, 0, 0, 240 - 1,240 - 1, HSV_BLACK, true);wait_ms(100);
    qp_flush(display);wait_ms(100);
    qp_rect(display, 50, 50, 200,200, HSV_WHITE, true);wait_ms(100);
    qp_flush(display);wait_ms(100);
	
    setPinOutput(C13);
	togglePin(C13);
	wait_ms(200);
	togglePin(C13);
	wait_ms(200);
	togglePin(C13);
	wait_ms(200);
	togglePin(C13);
	wait_ms(200);
}
/*
bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	for (int i = 0; i < 239; ++i) {
        qp_setpixel(display, 10, i, i, 255, 255);
		qp_setpixel(display, 15, i, i, 255, 255);
		qp_setpixel(display, 20, i, i, 255, 255);
    }
	qp_flush(display);
    return true;
}*/

/*
void housekeeping_task_kb(void) {
    static uint32_t last_draw = 0;
    if (timer_elapsed32(last_draw) > 33) { // Throttle to 30fps
        last_draw = timer_read32();
        // Draw a 240px high vertical rainbow line on X=0:
        for (int i = 0; i < 239; ++i) {
            qp_setpixel(display, 0, i, i, 255, 255);
        }
        qp_flush(display);
    }
}
*/
