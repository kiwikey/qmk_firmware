#ifndef ENCODER_H
#define ENCODER_H

#include "quantum.h"

#define ENCODER_FUNC_MAX 4

static const char * const encoder_func_name[ENCODER_FUNC_MAX+1] = {
	"Do nothing",
	"Media Volume",
	"Scroll (fast)",
	"Scroll (slow)",
	"Brightness"
};

bool process_encoder_rotate(uint8_t index, bool clockwise);
bool process_encoder_tap(void);

#endif /* ENCODER_H */
