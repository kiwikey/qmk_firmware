#pragma once

#include "quantum.h"
#include "as5600.h"

#define STEP_SIZE       128
#define MENU_STEP_SIZE  512

#define ENCODER_FUNC_MAX 5

extern int16_t accumulator;
extern uint32_t last_knob_movement_time;

void housekeeping_task_sensors_handler(void);
void keyboard_post_init_sensors_handler(void);
void magnetic_encoder_update_kb(bool direction);
bool process_encoder_rotate(bool clockwise);