#if defined(QUANTUM_PAINTER_ENABLE)

#include "quantum.h"
#include "qp_widget_status.h"

#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"
#include "features/eeprom_custom.h"

void widget_status_init(void) {
	widget_status_update();
}

void widget_status_update(void) {
	char buf1[24] = {0};
	char buf2[24] = {0};
	if (rgb_matrix_is_enabled()) {
		sprintf(buf1, "RGB MODE #%.2u", rgb_matrix_get_mode());
		sprintf(buf2, "BRIGHTNESS: %3u%%", rgb_matrix_get_val()*100/RGB_MATRIX_MAXIMUM_BRIGHTNESS);
	} else {
		sprintf(buf1, "RGB OFF");
		sprintf(buf2, "BRIGHTNESS: 0%%");
	}
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_POSX, WIDGET_STATUS_POSY,
						WIDGET_STATUS_FONT,
						buf1,
						HSV_WHITE,
						GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_POSX, WIDGET_STATUS_POSY + (WIDGET_STATUS_FONT->line_height+2) *1,
						WIDGET_STATUS_FONT,
						buf2,
						HSV_WHITE,
						GLOBAL_BG_COLOR);
	if (eepdata.display_timeout >= DISPLAY_TIMEOUT_NEVER) {
		sprintf(buf1, "SLEEP: NEVER");
	} else {
		sprintf(buf1, "SLEEP: %us", eepdata.display_timeout);
	}
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_POSX, WIDGET_STATUS_POSY + (WIDGET_STATUS_FONT->line_height+2) *2,
						WIDGET_STATUS_FONT,
						buf1,
						HSV_WHITE,
						GLOBAL_BG_COLOR);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
