#pragma once

#include "quantum.h"
#include "magnetic_encoder.h"

#define AS5600_MAX 4096

void housekeeping_task_sensors_handler(void);
void keyboard_post_init_sensors_handler(void);
void magnetic_encoder_update_user(magnetic_encoder_kind_t magnetic_encoder, bool direction);
