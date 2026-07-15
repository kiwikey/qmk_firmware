#if defined(QUANTUM_PAINTER_ENABLE)

#include "quantum.h"
#include "qp_widget_knob.h"
#include <lib/lib8tion/lib8tion.h>

#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"
#include "sensor/as5600.h"

// Screen position of the DOT as it was last actually drawn. Compared against
// on each update so that a run of small encoder ticks accumulates instead of
// each tick's tiny sensor-to-sensor delta being (mis)judged as "no movement".
static int last_drawn_x = 0;
static int last_drawn_y = 0;

// Draws the DOT at the encoder's current angle (does not erase anything first).
static void widget_knob_draw_dot(void) {
	int angle = 255 - (magnetic_encoder.new_angle >> 4);
	last_drawn_x = WIDGET_KNOB_CENTERX + (WIDGET_KNOB_DOT_ORBIT_RADIUS * (cos8(angle)-128)) / 128;
	last_drawn_y = WIDGET_KNOB_CENTERY - (WIDGET_KNOB_DOT_ORBIT_RADIUS * (sin8(angle)-128)) / 128;
	qp_circle(my_display, last_drawn_x, last_drawn_y,
			  WIDGET_KNOB_DOT_SIZE,
			  WIDGET_KNOB_DOT_COLOR,
			  WIDGET_KNOB_DOT_IS_FILLED);
}

// Draws the "!" centered in the knob (does not erase the DOT first). The
// clear radius stays inside the DOT's orbit (orbit radius - dot size) so it
// never touches the DOT/ring, meaning it also safely wipes any leftover "!".
static void widget_knob_draw_missing(void) {
	qp_circle(my_display, WIDGET_KNOB_CENTERX, WIDGET_KNOB_CENTERY,
			  WIDGET_KNOB_CENTER_CLEAR_RADIUS,
			  WIDGET_KNOB_BG_COLOR, true);
	qp_drawtext_recolor_center(my_display,
								WIDGET_KNOB_CENTERX, WIDGET_KNOB_CENTERY,
								WIDGET_KNOB_MISSING_FONT, "!",
								WIDGET_KNOB_DOT_COLOR,
								WIDGET_KNOB_BG_COLOR);
}

void widget_knob_init(void) {
	qp_circle(my_display,
			  WIDGET_KNOB_CENTERX + UI_WIDGET_SHADOW,
			  WIDGET_KNOB_CENTERY + UI_WIDGET_SHADOW,
			  WIDGET_KNOB_RADIUS,
			  UI_COLOR_SHADOW, true);
	qp_circle(my_display,
			  WIDGET_KNOB_CENTERX, WIDGET_KNOB_CENTERY,
			  WIDGET_KNOB_RADIUS,
			  WIDGET_KNOB_BG_COLOR, WIDGET_KNOB_IS_FILLED);
	qp_circle(my_display,
			  WIDGET_KNOB_CENTERX, WIDGET_KNOB_CENTERY,
			  WIDGET_KNOB_RADIUS,
			  HSV_YELLOW, FALSE);

	if (magnetic_encoder.is_present) {
		widget_knob_draw_dot();
	} else {
		widget_knob_draw_missing();
	}
}

// Call when the magnet is (re)detected: clears the "!" and shows the DOT again.
void widget_knob_show_dot(void) {
	qp_circle(my_display, WIDGET_KNOB_CENTERX, WIDGET_KNOB_CENTERY,
			  WIDGET_KNOB_CENTER_CLEAR_RADIUS,
			  WIDGET_KNOB_BG_COLOR, true);
	widget_knob_draw_dot();
}

// Call when the magnet is lost: clears the DOT and shows "!" instead.
void widget_knob_show_missing(void) {
	qp_circle(my_display, last_drawn_x, last_drawn_y,
			  WIDGET_KNOB_DOT_SIZE,
			  WIDGET_KNOB_BG_COLOR,
			  WIDGET_KNOB_DOT_IS_FILLED);
	widget_knob_draw_missing();
}

void widget_knob_update(uint16_t last_pos, uint16_t new_pos) {
	(void)last_pos; // erase position now comes from last_drawn_x/y, not the sensor's previous tick

	int angle = 255 - (new_pos >> 4);
	int new_x = WIDGET_KNOB_CENTERX + (WIDGET_KNOB_DOT_ORBIT_RADIUS * (cos8(angle)-128)) / 128;
	int new_y = WIDGET_KNOB_CENTERY - (WIDGET_KNOB_DOT_ORBIT_RADIUS * (sin8(angle)-128)) / 128;

	// Skip the redraw if the dot hasn't moved far enough since it was last
	// actually drawn: single ticks only shift it ~1-2px, so comparing against
	// the sensor's immediately-previous angle (as before) always looked like
	// "no movement" and froze the dot. Comparing against the last *drawn*
	// position lets small ticks accumulate until a redraw is actually due.
	int dx = new_x - last_drawn_x;
	int dy = new_y - last_drawn_y;
	if ((dx * dx + dy * dy) < (WIDGET_KNOB_DOT_RESOLUTION * WIDGET_KNOB_DOT_RESOLUTION)) {
		return;
	}

	// clear the old DOT
	qp_circle(my_display, last_drawn_x, last_drawn_y,
			  WIDGET_KNOB_DOT_SIZE,
			  WIDGET_KNOB_BG_COLOR,
			  WIDGET_KNOB_DOT_IS_FILLED);
	// draw the new DOT
	qp_circle(my_display, new_x, new_y,
			  WIDGET_KNOB_DOT_SIZE,
			  WIDGET_KNOB_DOT_COLOR,
			  WIDGET_KNOB_DOT_IS_FILLED);

	last_drawn_x = new_x;
	last_drawn_y = new_y;
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
