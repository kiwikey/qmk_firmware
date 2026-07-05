#pragma once

// The Knob itself
#define WIDGET_KNOB_CENTERX           256
#define WIDGET_KNOB_CENTERY           140
#define WIDGET_KNOB_RADIUS            50
#define WIDGET_KNOB_IS_FILLED         FALSE

// Knob's indicator, aka the "DOT"
#define WIDGET_KNOB_DOT_SIZE          10
#define WIDGET_KNOB_DOT_ORBIT_RADIUS  (WIDGET_KNOB_RADIUS - 20)
#define WIDGET_KNOB_BG_COLOR          HSV_BLACK
#define WIDGET_KNOB_DOT_COLOR         HSV_RED

void widget_knob_init(void);
void widget_knob_update(uint16_t last_pos, uint16_t new_pos);
