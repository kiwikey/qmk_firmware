#include QMK_KEYBOARD_H

#if defined(OLED_ENABLE)

#ifndef OLED_LOGO
#define OLED_LOGO

void render_qmk_logofull(uint8_t col, uint8_t row);

#define LOGO_WIDTH  24
#define LOGO_HEIGHT 24

static const uint8_t qmk_logofull[3] = {
	0x81,
    0xA1,
    0xC1
};

void render_qmk_logofull(uint8_t col, uint8_t row) {
	uint8_t temp;
	for (uint8_t i = 0; i < 3; i++) {
		oled_set_cursor(col, row+i);
		temp = qmk_logofull[i];
		for (uint8_t j = 0; j < 19; j++) {
			oled_write_char(temp++, false);
		}
	}
}

#endif /* OLED_LOGO */

#endif // defined(OLED_ENABLE)
