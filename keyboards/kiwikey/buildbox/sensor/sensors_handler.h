#pragma once

#include "quantum.h"
#include "as5600.h"

#define MENU_STEP_SIZE  512
#define KNOB_ACCUMULATOR_LIMIT  AS5600_MAX_VALUE // |knob_accumulator| cap = one full knob turn - far above any step size, far below INT16_MAX

#define KNOB_MAX_TAPS_PER_EVENT 2 // most knob-function keycodes (volume/scroll) sent per sensor event, see knob_on_rotation()

#define KNOB_SENSITIVITY_LOW    0 // needs the most rotation per activation
#define KNOB_SENSITIVITY_MEDIUM 1
#define KNOB_SENSITIVITY_HIGH   2 // needs the least rotation per activation
#define KNOB_SENSITIVITY_COUNT  3

extern int16_t knob_accumulator;
extern uint32_t last_knob_movement_time;

void housekeeping_task_sensors_handler(void);
void keyboard_post_init_sensors_handler(void);
