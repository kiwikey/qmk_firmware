#pragma once

// The Knob itself
#define WIDGET_KNOB_CENTERX           265
#define WIDGET_KNOB_CENTERY           110
#define WIDGET_KNOB_RADIUS            50
#define WIDGET_KNOB_OUTTER_THICKNESS  2
#define WIDGET_KNOB_OUTTER_COLOR      GLOBAL_THEME_COLOR

// Knob's indicator, aka the "DOT"
#define WIDGET_KNOB_DOT_SIZE          8
#define WIDGET_KNOB_DOT_IS_FILLED     TRUE
#define WIDGET_KNOB_DOT_ORBIT_RADIUS  (WIDGET_KNOB_RADIUS - 14)
#define WIDGET_KNOB_BG_COLOR          HSV_BLACK
#define WIDGET_KNOB_DOT_COLOR         GLOBAL_THEME_COLOR
// Minimum on-screen movement (px) before the DOT is redrawn. Lower = smoother
// rotation but more frequent erase/redraw flicker; higher = choppier but less flicker.
#define WIDGET_KNOB_DOT_RESOLUTION    5

// Shown centered in the knob instead of the DOT when the AS5600 reports no magnet.
#define WIDGET_KNOB_FONT              nanoplex32
// Radius cleared before (re)drawing the center content. Stays under
// (orbit radius - dot size) so it never overlaps the DOT/ring.
#define WIDGET_KNOB_CENTER_CLEAR_RADIUS   (WIDGET_KNOB_DOT_ORBIT_RADIUS - WIDGET_KNOB_DOT_SIZE - 5)

void widget_knob_init(void);
void widget_knob_update(uint16_t last_pos, uint16_t new_pos);
void widget_knob_show_dot(void);
void widget_knob_show_missing(void);
void widget_knob_show_func(void);

// Just the static ring (no dot/missing indicator, no live encoder state) at an
// arbitrary position - for use outside the idle screen's fixed WIDGET_KNOB_CENTERX/Y
// (e.g. display/widgets/tutorial.c). widget_knob_init() uses this too.
void widget_knob_draw_ring(uint16_t centerx, uint16_t centery, uint16_t radius);