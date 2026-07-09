#pragma once

/*** Draw a round rectangle
	'corner'    = rounded corner's radius in pixel
	'roundtop'    enables top-left and top-right
	'roundbottom' enables bottom-left and bottom-right
***/
bool qp_roundrect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom,
                  uint8_t hue, uint8_t sat, uint8_t val, bool filled,
                  uint16_t corner, bool roundtop, bool roundbottom);

int16_t qp_drawtext_recolor_center(painter_device_t device, uint16_t x, uint16_t y, painter_font_handle_t font, const char *str,
								   uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg);

void rgb_matrix_set_color_hsv(uint8_t index, hsv_t hsv);