#if defined(QUANTUM_PAINTER_ENABLE)

#include "screensaver_matrix_rain.h"

#include <stdlib.h>
#include "display/widgets/qp_widget_screensaver.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"

static const char charset_1[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; // all confirmed present in font_oled
static const char charset_2[] = "01";

typedef struct {
	int16_t y;           // pixel y of the head glyph's top-left corner (can be negative while falling in from above)
	char    head_char;   // last character drawn as the head, so it can be repainted as a trail cell next step
	uint8_t trail_len;    // rows behind the head that stay lit
	uint8_t speed;        // frames between steps for this column - lower = faster
	uint8_t frame_accum;  // frames elapsed since this column's last step
} column_t;

static column_t    columns[SCREENSAVER_COLS];
static const char *active_charset;
static uint8_t     active_charset_len;

static uint8_t rand_range(uint8_t min, uint8_t max) {
	return min + (uint8_t)(rand() % (max - min + 1));
}

/***  (Re)roll one column's speed/trail-length and send it back above the screen.
	'initial' spreads the very first batch across the whole screen height (and a
	bit above it), so the effect looks already running instead of starting from
	a blank screen; every later respawn starts fully off-screen, staggered.
***/
static void column_reset(uint8_t col, bool initial) {
	column_t *c = &columns[col];
	c->trail_len   = rand_range(SCREENSAVER_MATRIX_TRAIL_MIN, SCREENSAVER_MATRIX_TRAIL_MAX);
	c->speed       = rand_range(SCREENSAVER_MATRIX_SPEED_MIN, SCREENSAVER_MATRIX_SPEED_MAX);
	c->frame_accum = (uint8_t)(rand() % c->speed);
	c->head_char   = '\0';

	int16_t above = -(int16_t)(c->trail_len * SCREENSAVER_GLYPH_H);
	if (initial) {
		c->y = above + (int16_t)(rand() % (ST7789_HEIGHT - above));
	} else {
		c->y = above - (int16_t)(rand() % ST7789_HEIGHT);
	}
}

static void common_init(bool initial) {
	qp_rect(my_display, 0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1, GLOBAL_BG_COLOR, true);
	for (uint8_t col = 0; col < SCREENSAVER_COLS; col++) {
		column_reset(col, initial);
	}
	qp_flush(my_display);
}

void screensaver_matrix_rain_1_init(bool initial) {
	active_charset     = charset_1;
	active_charset_len = sizeof(charset_1) - 1;
	common_init(initial);
}

void screensaver_matrix_rain_2_init(bool initial) {
	active_charset     = charset_2;
	active_charset_len = sizeof(charset_2) - 1;
	common_init(initial);
}

void screensaver_matrix_rain_step(void) {
	for (uint8_t col = 0; col < SCREENSAVER_COLS; col++) {
		column_t *c = &columns[col];
		c->frame_accum++;
		if (c->frame_accum < c->speed) continue;
		c->frame_accum = 0;

		int16_t x      = col * SCREENSAVER_GLYPH_W;
		char    buf[2] = {0};

		// The current head becomes a normal (dimmer) trail cell once it's no longer the newest
		if (c->head_char && c->y >= 0 && c->y < ST7789_HEIGHT) {
			buf[0] = c->head_char;
			qp_drawtext_recolor(my_display, x, c->y, SCREENSAVER_FONT, buf, SCREENSAVER_MATRIX_TRAIL_COLOR, GLOBAL_BG_COLOR);
		}

		// Erase the cell scrolling out of the trail's tail
		int16_t erase_y = c->y - (int16_t)(c->trail_len * SCREENSAVER_GLYPH_H);
		if (erase_y >= 0 && erase_y < ST7789_HEIGHT) {
			qp_rect(my_display, x, erase_y, x + SCREENSAVER_GLYPH_W - 1, erase_y + SCREENSAVER_GLYPH_H - 1, GLOBAL_BG_COLOR, true);
		}

		c->y += SCREENSAVER_GLYPH_H;
		c->head_char = active_charset[rand() % active_charset_len];

		if (c->y >= 0 && c->y < ST7789_HEIGHT) {
			buf[0] = c->head_char;
			qp_drawtext_recolor(my_display, x, c->y, SCREENSAVER_FONT, buf, SCREENSAVER_MATRIX_HEAD_COLOR, GLOBAL_BG_COLOR);
		}

		// Respawn once the whole trail (down to its tail) has scrolled past the bottom
		if (c->y - (int16_t)(c->trail_len * SCREENSAVER_GLYPH_H) >= ST7789_HEIGHT) {
			column_reset(col, false);
		}
	}
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
