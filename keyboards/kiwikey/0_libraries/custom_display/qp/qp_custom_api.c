#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp.h"
#include "qp_custom_api.h"

extern painter_device_t my_display;

bool qp_roundrect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint8_t hue, uint8_t sat, uint8_t val, bool filled, uint16_t corner, bool roundtop, bool roundbottom) {	
	qp_rect(my_display, left, top+corner, right, bottom-corner, hue, sat, val, filled);
	if (roundtop) {
		qp_rect(my_display, left+corner, top, right-corner, bottom, hue, sat, val, filled);
		qp_circle(my_display, left+corner, top+corner, corner, hue, sat, val, filled);
		qp_circle(my_display, right-corner, top+corner, corner, hue, sat, val, filled);
	} else {
		qp_rect(my_display, left, top, right, top+corner, hue, sat, val, filled);
	}
	if (roundbottom) {
		qp_rect(my_display, left+corner, bottom-corner, right-corner, bottom, hue, sat, val, filled);
		qp_circle(my_display, left+corner, bottom-corner, corner, hue, sat, val, filled);
		qp_circle(my_display, right-corner, bottom-corner, corner, hue, sat, val, filled);
	} else {
		qp_rect(my_display, left, bottom-corner, right, bottom, hue, sat, val, filled);
	}
	return true;
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
