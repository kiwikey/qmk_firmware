#include "sensors_handler.h"

#include <lib/lib8tion/lib8tion.h>

#include "features/eeprom_custom.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"

static uint16_t last_pos = 0;
static int16_t  accumulator = 0;

void keyboard_post_init_sensors_handler(void) {
	keyboard_post_init_magnetic_encoder();
}

void housekeeping_task_sensors_handler(void) {
	housekeeping_task_magnetic_encoder();
}

void magnetic_encoder_update_kb(bool direction) {
	uint16_t pos = as5600_read_angle();
	int16_t delta = (int16_t)pos - (int16_t)last_pos;

    // Handle wrap-around
    if (delta > 2048)
        delta -= 4096;
    else if (delta < -2048)
        delta += 4096;

    accumulator += delta;
    printf("accumulator = %d \n", accumulator);
	widget_knob_update(last_pos, pos);

    while (accumulator >= STEP_SIZE) {
        tap_code16(dynamic_keymap_get_keycode(0, 0, 0));
        accumulator -= STEP_SIZE;
    }
    while (accumulator <= -STEP_SIZE) {
        tap_code16(dynamic_keymap_get_keycode(0, 0, 1));
        accumulator += STEP_SIZE;
    }

	last_pos = pos;
}
