#if defined(QUANTUM_PAINTER_ENABLE)

#include <qp.h>
#include "qp_widget_knob.h"
#include "../qp_custom_api.h"
#include "../defines.h"

extern painter_device_t my_display;

void widget_knob_init(void) {
	// qp_rect(my_display, 200, 10, 300, 300, HSV_WHITE, false);
	qp_circle(my_display, 256, 140, 50, HSV_GOLD, true);	
}

void widget_knob_update(uint8_t col, uint8_t row) {

}

#endif // defined(QUANTUM_PAINTER_ENABLE)
