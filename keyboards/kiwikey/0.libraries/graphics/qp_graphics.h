#pragma once

#ifndef QP_GRAPHICS
#define QP_GRAPHICS

static const char * const layer_name[DYNAMIC_KEYMAP_LAYER_COUNT] = {
	"Default",
	" Raise ",
	" Lower ",
	" User  "
};

void render_ui(void);

#endif /* QP_GRAPHICS */