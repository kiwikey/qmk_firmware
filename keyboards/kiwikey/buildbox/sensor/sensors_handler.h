#pragma once

#include "quantum.h"
#include "as5600.h"

#define MENU_STEP_SIZE  512

#define THEME_COLOR_HUE_STEP 8 // eepdata.theme_hue step per encoder tick in MENU_THEME_COLOR (wraps via uint8_t overflow, 0..255 is a full hue circle)

#define KNOB_SENSITIVITY_LOW    0 // needs the most rotation per activation
#define KNOB_SENSITIVITY_MEDIUM 1
#define KNOB_SENSITIVITY_HIGH   2 // needs the least rotation per activation
#define KNOB_SENSITIVITY_COUNT  3

static const uint16_t knob_sensitivity_step[KNOB_SENSITIVITY_COUNT] = {
	256, // LOW
	128, // MEDIUM
	64,  // HIGH
};

// Menu display text, indexed by eepdata.knob_sensitivity.
static const char * const knob_sensitivity_menu_text[KNOB_SENSITIVITY_COUNT] = {
	"LOW",
	"MEDIUM",
	"HIGH",
};

extern int16_t accumulator;
extern uint32_t last_knob_movement_time;

void housekeeping_task_sensors_handler(void);
void keyboard_post_init_sensors_handler(void);
void magnetic_encoder_update_kb(bool direction);
bool process_encoder_rotate(bool clockwise);