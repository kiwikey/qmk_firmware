#pragma once

// The Knob itself
#define WIDGET_KNOB_CENTERX           256
#define WIDGET_KNOB_CENTERY           130
#define WIDGET_KNOB_RADIUS            50
#define WIDGET_KNOB_IS_FILLED         TRUE

// Knob's indicator, aka the "DOT"
#define WIDGET_KNOB_DOT_SIZE          10
#define WIDGET_KNOB_DOT_IS_FILLED     TRUE
#define WIDGET_KNOB_DOT_ORBIT_RADIUS  (WIDGET_KNOB_RADIUS - 15)
#define WIDGET_KNOB_BG_COLOR          HSV_BLACK
#define WIDGET_KNOB_DOT_COLOR         HSV_RED
// Minimum on-screen movement (px) before the DOT is redrawn. Lower = smoother
// rotation but more frequent erase/redraw flicker; higher = choppier but less flicker.
#define WIDGET_KNOB_DOT_RESOLUTION    5

// Shown centered in the knob instead of the DOT when the AS5600 reports no magnet.
#define WIDGET_KNOB_MISSING_FONT          robotobold25
// Radius cleared before (re)drawing the center content. Stays under
// (orbit radius - dot size) so it never overlaps the DOT/ring.
#define WIDGET_KNOB_CENTER_CLEAR_RADIUS   (WIDGET_KNOB_DOT_ORBIT_RADIUS - WIDGET_KNOB_DOT_SIZE - 5)

void widget_knob_init(void);
void widget_knob_update(uint16_t last_pos, uint16_t new_pos);
void widget_knob_show_dot(void);
void widget_knob_show_missing(void);
