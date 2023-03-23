#include QMK_KEYBOARD_H
#include "qp.h"

#include "sport40.h"
#include "sport40_graphics.h"

#include "pusheen_240px.qgf.h"

static painter_image_handle_t gif_pusheen;
static deferred_token         my_anim;

void keyboard_post_init_display(void) {
	gif_pusheen = qp_load_image_mem(gfx_pusheen_240px);
	
	my_anim = qp_animate(my_display, 0, 0, gif_pusheen);
}
