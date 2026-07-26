#if defined(QUANTUM_PAINTER_ENABLE)

#include "quantum.h"
#include "qp_widget_status.h"

#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"

void widget_status_init(void) {
	char buf1[15] = {0};
	sprintf(buf1, "Mode #1 *53%%");
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_POSX, WIDGET_STATUS_POSY,
						WIDGET_STATUS_FONT,
						buf1,
						HSV_WHITE,
						HSV_BLACK);
	// sprintf(buf1, "UPTIME:");
	// qp_drawtext_recolor(my_display,
	// 					WIDGET_STATUS_POSX, WIDGET_STATUS_POSY + 15,
	// 					WIDGET_STATUS_FONT,
	// 					buf1,
	// 					HSV_WHITE,
	// 					HSV_BLACK);
}

void widget_status_update(uint16_t last_pos, uint16_t new_pos) {

}

#endif // defined(QUANTUM_PAINTER_ENABLE)
