#if defined(QUANTUM_PAINTER_ENABLE)

#include "quantum.h"
#include "qp_widget_status.h"

#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"

void widget_status_init(void) {
	widget_status_update();
}

void widget_status_update(void) {
	char buf1[50] = {0};
	if (rgb_matrix_is_enabled()) {
		sprintf(buf1, "MODE #%.2u *%3u%%", rgb_matrix_get_mode(), rgb_matrix_get_val()*100/RGB_MATRIX_MAXIMUM_BRIGHTNESS);
	} else {
		sprintf(buf1, "RGB OFF");
	}
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_POSX, WIDGET_STATUS_POSY,
						WIDGET_STATUS_FONT,
						buf1,
						HSV_WHITE,
						GLOBAL_BG_COLOR);
}

void widget_status_render_uptime(void) {
	static uint32_t last_minutes = UINT32_MAX; // forces the first draw
	uint32_t minutes = timer_read32() / 60000;
	if (minutes == last_minutes) return;
	last_minutes = minutes;

	char buf1[20] = {0};
	if (minutes < 60) {
		sprintf(buf1, "UPTIME: %lumins", minutes);
	} else {
		sprintf(buf1, "UPTIME: %luh %02lum", minutes / 60, minutes % 60);
	}
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_POSX, WIDGET_STATUS_POSY + 15,
						WIDGET_STATUS_FONT,
						buf1,
						HSV_WHITE,
						GLOBAL_BG_COLOR);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
