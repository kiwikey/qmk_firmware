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
}

void widget_status_update(uint16_t last_pos, uint16_t new_pos) {

}

void widget_status_render_uptime(void) {
	static uint32_t last_minutes = UINT32_MAX; // forces the first draw
	uint32_t minutes = timer_read32() / 60000;
	if (minutes == last_minutes) return;
	last_minutes = minutes;

	char buf1[20] = {0};
	if (minutes < 60) {
		sprintf(buf1, "UPTIME: %lu mins", minutes);
	} else {
		sprintf(buf1, "UPTIME: %lu h %02lum", minutes / 60, minutes % 60);
	}
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_POSX, WIDGET_STATUS_POSY + 15,
						WIDGET_STATUS_FONT,
						buf1,
						HSV_WHITE,
						HSV_BLACK);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
