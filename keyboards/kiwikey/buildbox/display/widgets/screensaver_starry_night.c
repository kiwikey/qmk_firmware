#if defined(QUANTUM_PAINTER_ENABLE)

#include "screensaver_starry_night.h"

#include <stdlib.h>
#include "display/widgets/qp_widget_screensaver.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"

static const char starry_charset[] = ".+";

typedef struct {
	int16_t x, y;
	char    glyph;
	uint8_t brightness;   // current V (0-255)
	int8_t  direction;    // +1 fading in, -1 fading out
	uint8_t speed;         // frames per brightness step
	uint8_t frame_accum;
} star_t;

static star_t stars[SCREENSAVER_STARRY_COUNT];

/***  (Re)roll one star's position/glyph/speed.
	'initial' starts some stars already mid-fade (random brightness, either
	direction) so the sky doesn't all begin from black and twinkle in visible
	unison; every later respawn (once a star has fully faded out) starts
	invisible and fades back in, at a new position with a new duration.
***/
static void star_respawn(uint8_t i, bool initial) {
	star_t *s = &stars[i];
	s->x     = (int16_t)(rand() % (ST7789_WIDTH - SCREENSAVER_GLYPH_W));
	s->y     = (int16_t)(rand() % (ST7789_HEIGHT - SCREENSAVER_GLYPH_H));
	s->glyph = starry_charset[rand() % (sizeof(starry_charset) - 1)];
	s->speed       = SCREENSAVER_STARRY_SPEED_MIN + (uint8_t)(rand() % (SCREENSAVER_STARRY_SPEED_MAX - SCREENSAVER_STARRY_SPEED_MIN + 1));
	s->frame_accum = (uint8_t)(rand() % s->speed);

	if (initial) {
		s->brightness = (uint8_t)(rand() % 256);
		s->direction  = (rand() & 0x01) ? 1 : -1;
	} else {
		s->brightness = 0;
		s->direction  = 1;
	}
}

void screensaver_starry_night_init(bool initial) {
	qp_rect(my_display, 0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1, GLOBAL_BG_COLOR, true);
	for (uint8_t i = 0; i < SCREENSAVER_STARRY_COUNT; i++) {
		star_respawn(i, initial);
	}
	qp_flush(my_display);
}

void screensaver_starry_night_step(void) {
	char buf[2] = {0};
	for (uint8_t i = 0; i < SCREENSAVER_STARRY_COUNT; i++) {
		star_t *s = &stars[i];
		s->frame_accum++;
		if (s->frame_accum < s->speed) continue;
		s->frame_accum = 0;

		int16_t next        = (int16_t)s->brightness + s->direction * SCREENSAVER_STARRY_FADE_STEP;
		bool    fully_faded = false;
		if (next >= 255) {
			next          = 255;
			s->direction  = -1;
		} else if (next <= 0) {
			next        = 0;
			fully_faded = true;
		}
		s->brightness = (uint8_t)next;

		buf[0] = s->glyph;
		qp_drawtext_recolor(my_display, s->x, s->y, SCREENSAVER_STARRY_FONT, buf, 0, 0, s->brightness, GLOBAL_BG_COLOR);

		if (fully_faded) {
			star_respawn(i, false); // new position, new duration - starts at 0, fading back in
		}
	}
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
