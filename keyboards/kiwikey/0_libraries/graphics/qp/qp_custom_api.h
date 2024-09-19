#pragma once

#ifndef QP_CUSTOM_API
#define QP_CUSTOM_API

/*** Draw a round rectangle
	'corner'    = rounded corner's radius in pixel
	'roundtop'    enables top-left and top-right
	'roundbottom' enables bottom-left and bottom-right
***/
bool qp_roundrect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom,
                  uint8_t hue, uint8_t sat, uint8_t val, bool filled,
                  uint16_t corner, bool roundtop, bool roundbottom);

#endif /* QP_CUSTOM_API */