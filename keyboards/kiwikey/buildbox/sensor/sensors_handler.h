#pragma once

#include "quantum.h"
#include "as5600.h"

void housekeeping_task_sensors_handler(void);
void keyboard_post_init_sensors_handler(void);
void magnetic_encoder_update_user(bool direction);
