#if defined(QUANTUM_PAINTER_ENABLE)

#include "quantum.h"
#include "qp_widget_knob.h"
#include <lib/lib8tion/lib8tion.h>

#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"

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
}

void widget_knob_update(uint16_t last_pos, uint16_t new_pos) {
	int angle, x, y;
	// clear the old DOT
	angle = 255 - (last_pos >> 4);
	x = WIDGET_KNOB_CENTERX + (WIDGET_KNOB_DOT_ORBIT_RADIUS * (cos8(angle)-128)) / 128;
	y = WIDGET_KNOB_CENTERY - (WIDGET_KNOB_DOT_ORBIT_RADIUS * (sin8(angle)-128)) / 128;
	qp_circle(my_display, x, y,
			  WIDGET_KNOB_DOT_SIZE,
			  WIDGET_KNOB_BG_COLOR,
			  WIDGET_KNOB_IS_FILLED);
	// draw the new DOT
	angle = 255 - (new_pos >> 4);
	x = WIDGET_KNOB_CENTERX + (WIDGET_KNOB_DOT_ORBIT_RADIUS * (cos8(angle)-128)) / 128;
	y = WIDGET_KNOB_CENTERY - (WIDGET_KNOB_DOT_ORBIT_RADIUS * (sin8(angle)-128)) / 128;
	qp_circle(my_display, x, y,
			  WIDGET_KNOB_DOT_SIZE,
			  WIDGET_KNOB_DOT_COLOR,
			  WIDGET_KNOB_IS_FILLED);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
