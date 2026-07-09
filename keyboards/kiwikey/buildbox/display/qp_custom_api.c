#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp.h"
#include "qp_custom_api.h"
#include "display/qp_graphics.h"

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

int16_t qp_drawtext_recolor_center(painter_device_t device, uint16_t x, uint16_t y, painter_font_handle_t font, const char *str,
								   uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
	return qp_drawtext_recolor(device,
							   x - qp_textwidth(font, str)/2,
							   y - (font->line_height)/2,
							   font, str,
							   hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)

void rgb_matrix_set_color_hsv(uint8_t index, hsv_t hsv) {
	hsv.v = rgb_matrix_get_val(); // sync brightness to system's brightness, if not, max brightness (255) will be set
    rgb_t rgb = hsv_to_rgb(hsv);
    rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
}
