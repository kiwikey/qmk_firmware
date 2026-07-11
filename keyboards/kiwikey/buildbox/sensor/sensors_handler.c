#include "sensors_handler.h"

#include <lib/lib8tion/lib8tion.h>

#include "features/eeprom_custom.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"

static int16_t accumulator = 0;

void keyboard_post_init_sensors_handler(void) {
	keyboard_post_init_magnetic_encoder();
}

void housekeeping_task_sensors_handler(void) {
	housekeeping_task_magnetic_encoder();
}

// Called from process_magnetic_encoder() (as5600.c) once a movement past
// DEG_MARGIN_AS5600 has already been read and validated. `direction` is
// the sign of that movement: true = CW, false = CCW. This function does
// NOT touch the sensor itself, avoiding a second, racy I2C read per tick.
void magnetic_encoder_update_kb(bool direction) {
	uint16_t distance = get_distance(&magnetic_encoder);

	accumulator += direction ? (int16_t)distance : -(int16_t)distance;

#ifdef CONSOLE_ENABLE
	printf("accumulator = %d \n", accumulator);
#endif

	widget_knob_update(magnetic_encoder.prev_angle, magnetic_encoder.new_angle);

    while (accumulator >= STEP_SIZE) {
        tap_code16(dynamic_keymap_get_keycode(0, 0, 0));
        accumulator -= STEP_SIZE;
    }
    while (accumulator <= -STEP_SIZE) {
        tap_code16(dynamic_keymap_get_keycode(0, 0, 1));
        accumulator += STEP_SIZE;
    }
}