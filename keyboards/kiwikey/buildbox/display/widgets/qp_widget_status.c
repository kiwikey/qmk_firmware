#if defined(QUANTUM_PAINTER_ENABLE)

#include "quantum.h"
#include "qp_widget_status.h"

#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"
#include "features/eeprom_custom.h"

void widget_status_init(void) {
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_POSX,
						WIDGET_STATUS_POSY,
						WIDGET_STATUS_FONT,
						WIDGET_STATUS_LABEL1,
						WIDGET_STATUS_LABEL_COLOR,
						GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_POSX,
						WIDGET_STATUS_POSY + (WIDGET_STATUS_FONT->line_height+2) *1,
						WIDGET_STATUS_FONT,
						WIDGET_STATUS_LABEL2,
						WIDGET_STATUS_LABEL_COLOR,
						GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_POSX,
						WIDGET_STATUS_POSY + (WIDGET_STATUS_FONT->line_height+2) *2,
						WIDGET_STATUS_FONT,
						WIDGET_STATUS_LABEL3,
						HSV_WHITE,
						GLOBAL_BG_COLOR);
	widget_status_update();
}

void widget_status_update(void) {
	// Clear old data
	qp_rect(my_display,
			WIDGET_STATUS_VALUE_POSX, WIDGET_STATUS_POSY,
			DISPLAY_WIDTH-1, WIDGET_STATUS_POSY + (WIDGET_STATUS_FONT->line_height+2)*2 + WIDGET_STATUS_FONT->line_height,
			GLOBAL_BG_COLOR, true);
	// Draw new data
	char buf1[24] = {0};
	char buf2[24] = {0};
	if (rgb_matrix_is_enabled()) {
		sprintf(buf1, "#%.2u", rgb_matrix_get_mode());
		sprintf(buf2, "%u%%", rgb_matrix_get_val()*100/RGB_MATRIX_MAXIMUM_BRIGHTNESS);
	} else {
		sprintf(buf1, "OFF");
		sprintf(buf2, "0%%");
	}
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_VALUE_POSX,
						WIDGET_STATUS_POSY,
						WIDGET_STATUS_FONT,
						buf1,
						WIDGET_STATUS_VALUE_COLOR,
						GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_VALUE_POSX,
						WIDGET_STATUS_POSY + (WIDGET_STATUS_FONT->line_height+2) *1,
						WIDGET_STATUS_FONT,
						buf2,
						WIDGET_STATUS_VALUE_COLOR,
						GLOBAL_BG_COLOR);
	{
		uint8_t idx = eepdata.display_timeout < DISPLAY_TIMEOUT_COUNT ? eepdata.display_timeout : DISPLAY_TIMEOUT_NEVER_INDEX;
		sprintf(buf1, "%s", display_timeout_text[idx]);
	}
	qp_drawtext_recolor(my_display,
						WIDGET_STATUS_VALUE_POSX,
						WIDGET_STATUS_POSY + (WIDGET_STATUS_FONT->line_height+2) *2,
						WIDGET_STATUS_FONT,
						buf1,
						HSV_CYAN,
						GLOBAL_BG_COLOR);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
