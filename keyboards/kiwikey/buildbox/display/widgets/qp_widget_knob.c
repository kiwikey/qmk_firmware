#if defined(QUANTUM_PAINTER_ENABLE)

#include <qp.h>
#include "qp_widget_knob.h"
#include "display/defines.h"

extern painter_device_t my_display;

void widget_knob_init(void) {
	qp_circle(my_display, 256, 140, 50, HSV_GOLD, true);	
}

void widget_knob_update(uint8_t col, uint8_t row) {

}

#endif // defined(QUANTUM_PAINTER_ENABLE)
