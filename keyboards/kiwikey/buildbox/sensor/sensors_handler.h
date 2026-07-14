#pragma once

#include "quantum.h"
#include "as5600.h"

#define STEP_SIZE       128
#define MENU_STEP_SIZE  512

extern int16_t accumulator;

void housekeeping_task_sensors_handler(void);
void keyboard_post_init_sensors_handler(void);
void magnetic_encoder_update_kb(bool direction);
bool process_encoder_rotate(bool clockwise);