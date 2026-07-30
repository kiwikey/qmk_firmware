#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp.h"
#include "qp_internal.h"
#include "qp_comms.h"
#include "qp_draw.h"
#include "qp_custom_api.h"
#include "display/qp_graphics.h"

int16_t qp_drawtext_recolor_center(painter_device_t device, uint16_t x, uint16_t y, painter_font_handle_t font, const char *str,
								   uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
	return qp_drawtext_recolor(device,
							   x - qp_textwidth(font, str)/2,
							   y - (font->line_height)/2,
							   font, str,
							   hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
}

static bool qp_roundrect_corner(painter_device_t device, uint16_t cx, uint16_t cy, uint16_t radius,
                                 bool filled, bool mirror_x, bool mirror_y) {
	int16_t x   = 0;
	int16_t y   = (int16_t)radius;
	int16_t err = ((5 - (radius >> 2)) >> 2);

	while (x <= y) {
		int16_t ax = mirror_x ? -x : x;
		int16_t ay = mirror_y ? -y : y;
		int16_t bx = mirror_x ? -y : y;
		int16_t by = mirror_y ? -x : x;

		if (filled) {
			// Each row runs from the arc edge in to the quadrant's flat edge (cx);
			// the rest of the row belongs to the body rects drawn by the caller.
			if (!qp_internal_fillrect_helper_impl(device, cx, cy + ay, cx + ax, cy + ay)) return false;
			if (!qp_internal_fillrect_helper_impl(device, cx, cy + by, cx + bx, cy + by)) return false;
		} else {
			if (!qp_internal_setpixel_impl(device, cx + ax, cy + ay)) return false;
			if (!qp_internal_setpixel_impl(device, cx + bx, cy + by)) return false;
		}

		x++;
		if (err < 0) {
			err += (x << 1) + 1;
		} else {
			y--;
			err += ((x - y) << 1) + 1;
		}
	}
	return true;
}

bool qp_roundrect(painter_device_t device,
                   uint16_t left, uint16_t top, uint16_t right, uint16_t bottom,
                   uint8_t hue, uint8_t sat, uint8_t val, bool filled,
                   uint16_t corner, bool roundtop, bool roundbottom) {
	painter_driver_t *driver = (painter_driver_t *)device;
	if (!driver || !driver->validate_ok) {
		return false;
	}

	uint16_t top_inset    = roundtop    ? corner : 0;
	uint16_t bottom_inset = roundbottom ? corner : 0;

	if (!qp_comms_start(device)) {
		return false;
	}

	// Single shape, single color: fill the pixdata buffer once and reuse it
	// for every subsequent rect/pixel write in this call.
	qp_internal_fill_pixdata(device, qp_internal_num_pixels_in_buffer(device), hue, sat, val);

	bool ret = true;
	if (filled) {
		// Full-width center band, plus the top/bottom corner-height strips
		// trimmed to sit between the two corner boxes.
		ret = qp_internal_fillrect_helper_impl(device, left, top + top_inset, right, bottom - bottom_inset);
		if (ret && roundtop) {
			ret = qp_internal_fillrect_helper_impl(device, left + corner, top, right - corner, top + corner);
		}
		if (ret && roundbottom) {
			ret = qp_internal_fillrect_helper_impl(device, left + corner, bottom - corner, right - corner, bottom);
		}
	} else {
		// Four straight edges, each trimmed to the non-rounded span.
		ret = qp_internal_fillrect_helper_impl(device, left + top_inset, top, right - top_inset, top) &&
		      qp_internal_fillrect_helper_impl(device, left + bottom_inset, bottom, right - bottom_inset, bottom) &&
		      qp_internal_fillrect_helper_impl(device, left, top + top_inset, left, bottom - bottom_inset) &&
		      qp_internal_fillrect_helper_impl(device, right, top + top_inset, right, bottom - bottom_inset);
	}

	if (ret && roundtop) {
		ret = qp_roundrect_corner(device, left + corner,  top + corner, corner, filled, true,  true) &&
		      qp_roundrect_corner(device, right - corner, top + corner, corner, filled, false, true);
	}
	if (ret && roundbottom) {
		ret = qp_roundrect_corner(device, left + corner,  bottom - corner, corner, filled, true,  false) &&
		      qp_roundrect_corner(device, right - corner, bottom - corner, corner, filled, false, false);
	}

	qp_comms_stop(device);
	return ret;
}

#endif // defined(QUANTUM_PAINTER_ENABLE)

void rgb_matrix_set_color_hsv(uint8_t index, hsv_t hsv) {
	hsv.v = rgb_matrix_get_val(); // sync brightness to system's brightness, if not, max brightness (255) will be set
    rgb_t rgb = hsv_to_rgb(hsv);
    rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
}

void toUppercase(const char *str, char *out, size_t outsize) {
    size_t i = 0;
    for (; str[i] != '\0' && i + 1 < outsize; i++) {
        out[i] = (str[i] >= 'a' && str[i] <= 'z') ? str[i] - 32 : str[i];
    }
    out[i] = '\0';
}