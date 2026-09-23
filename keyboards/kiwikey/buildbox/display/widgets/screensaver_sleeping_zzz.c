#if defined(QUANTUM_PAINTER_ENABLE)

#include "screensaver_sleeping_zzz.h"

#include <stdlib.h>
#include "display/widgets/qp_widget_screensaver.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"

typedef struct {
	int16_t                x, y;      // pixel position of the glyph's top-left corner
	uint8_t                glyph_w, glyph_h;
	painter_font_handle_t  font;
	uint8_t                speed;       // frames per upward step
	uint8_t                frame_accum;
} zzz_particle_t;

static zzz_particle_t particles[SCREENSAVER_ZZZ_COUNT];

/***  (Re)roll one particle's size/speed/x and send it below the screen.
	'initial' spreads the very first batch across the whole screen height, so
	the effect looks already running instead of starting from blank; every
	later respawn re-enters from below the bottom edge, staggered.
***/
static void particle_respawn(uint8_t i, bool initial) {
	zzz_particle_t *p = &particles[i];
	p->font    = (rand() & 0x01) ? SCREENSAVER_ZZZ_FONT_BIG : SCREENSAVER_ZZZ_FONT_SMALL;
	p->glyph_w = qp_textwidth(p->font, "Z");
	p->glyph_h = p->font->line_height;
	p->speed       = SCREENSAVER_ZZZ_SPEED_MIN + (uint8_t)(rand() % (SCREENSAVER_ZZZ_SPEED_MAX - SCREENSAVER_ZZZ_SPEED_MIN + 1));
	p->frame_accum = (uint8_t)(rand() % p->speed);
	p->x           = (int16_t)(rand() % (ST7789_WIDTH - p->glyph_w));

	if (initial) {
		p->y = (int16_t)(rand() % (ST7789_HEIGHT + p->glyph_h)) - p->glyph_h;
	} else {
		p->y = ST7789_HEIGHT + (int16_t)(rand() % ST7789_HEIGHT);
	}
}

void screensaver_sleeping_zzz_init(bool initial) {
	qp_rect(my_display, 0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1, GLOBAL_BG_COLOR, true);
	for (uint8_t i = 0; i < SCREENSAVER_ZZZ_COUNT; i++) {
		particle_respawn(i, initial);
	}
	qp_flush(my_display);
}

void screensaver_sleeping_zzz_step(void) {
	for (uint8_t i = 0; i < SCREENSAVER_ZZZ_COUNT; i++) {
		zzz_particle_t *p = &particles[i];
		p->frame_accum++;
		if (p->frame_accum < p->speed) continue;
		p->frame_accum = 0;

		// Erase the old glyph before moving
		qp_rect(my_display, p->x, p->y, p->x + p->glyph_w - 1, p->y + p->glyph_h - 1, GLOBAL_BG_COLOR, true);

		p->y -= p->glyph_h; // drift upward

		if (p->y + p->glyph_h < 0) { // fully off the top - loop back in from the bottom
			particle_respawn(i, false);
		} else {
			qp_drawtext_recolor(my_display, p->x, p->y, p->font, "Z", SCREENSAVER_ZZZ_COLOR, GLOBAL_BG_COLOR);
		}
	}
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
