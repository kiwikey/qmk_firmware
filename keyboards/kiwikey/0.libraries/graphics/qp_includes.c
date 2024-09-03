#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp_includes.h"

// FONTS
#include "resources/thintel15.qff.h"
#include "resources/thintel50.qff.h"
#include "resources/arial30.qff.h"
painter_font_handle_t  thintel;
painter_font_handle_t  thintel50;
painter_font_handle_t  arial30;

// IMAGES & GIFS
#include "resources/bg_240x240.qgf.h"
#include "resources/pikachu.qgf.h"
#include "resources/pusheen_240px.qgf.h"
#include "resources/anya01.qgf.h"
painter_image_handle_t img_bg_240x240;
painter_image_handle_t img_pikachu;
painter_image_handle_t img_anya01;
painter_image_handle_t gif_pusheen;
//deferred_token         my_anim;

// ICONS
#include "resources/icons/lock-caps-ON.qgf.h"
#include "resources/icons/lock-caps-OFF.qgf.h"
#include "resources/icons/lock-num-ON.qgf.h"
#include "resources/icons/lock-num-OFF.qgf.h"
#include "resources/icons/lock-scrl-ON.qgf.h"
#include "resources/icons/lock-scrl-OFF.qgf.h"
painter_image_handle_t lock_caps_on;
painter_image_handle_t lock_caps_off;
painter_image_handle_t lock_num_on;
painter_image_handle_t lock_num_off;
painter_image_handle_t lock_scrl_on;
painter_image_handle_t lock_scrl_off;

void qp_init_load_files(void) {
    thintel        = qp_load_font_mem(font_thintel15);
	thintel50      = qp_load_font_mem(font_thintel50);
    arial30        = qp_load_font_mem(font_arial30);

	img_bg_240x240 = qp_load_image_mem(gfx_bg_240x240);
    img_pikachu    = qp_load_image_mem(gfx_pikachu);
    img_anya01     = qp_load_image_mem(gfx_anya01);
    gif_pusheen    = qp_load_image_mem(gfx_pusheen_240px);
	
	lock_caps_on  = qp_load_image_mem(gfx_lock_caps_ON);
    lock_caps_off = qp_load_image_mem(gfx_lock_caps_OFF);
    lock_num_on   = qp_load_image_mem(gfx_lock_num_ON);
    lock_num_off  = qp_load_image_mem(gfx_lock_num_OFF);
    lock_scrl_on  = qp_load_image_mem(gfx_lock_scrl_ON);
    lock_scrl_off = qp_load_image_mem(gfx_lock_scrl_OFF);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
