#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp.h"
#include "qp_includes.h"

/* FONTS */
	#include "resources/thintel15.qff.h"
	#include "resources/roboto20.qff.h"
	#include "resources/roboto25.qff.h"
	#include "resources/robotobold25.qff.h"
	painter_font_handle_t  thintel;
	painter_font_handle_t  roboto20;
	painter_font_handle_t  roboto25;
	painter_font_handle_t  robotobold25;

/* IMAGES & GIFS */
	// #include "resources/pikachu.qgf.h"
	// #include "resources/pusheen_240px.qgf.h"
	// #include "resources/anya01.qgf.h"
	// #include "resources/gif_nyan120px.qgf.h"
	// #include "resources/gif_cat01.qgf.h"
	// #include "resources/gif_cat02.qgf.h"
	// #include "resources/gif_dog01.qgf.h"
	// painter_image_handle_t img_pikachu;
	// painter_image_handle_t img_anya01;
	// painter_image_handle_t gif_pusheen;
	// painter_image_handle_t gif_nyan120px;
	// painter_image_handle_t gif_cat01;
	// painter_image_handle_t gif_cat02;
	// painter_image_handle_t gif_dog01;
	// deferred_token         my_anim;

/* ICONS */
	// #include "resources/icons/lock-caps-ON.qgf.h"
	// #include "resources/icons/lock-caps-OFF.qgf.h"
	// #include "resources/icons/lock-num-ON.qgf.h"
	// #include "resources/icons/lock-num-OFF.qgf.h"
	// #include "resources/icons/lock-scrl-ON.qgf.h"
	// #include "resources/icons/lock-scrl-OFF.qgf.h"
	#include "resources/icons/ico32_brightness.qgf.h"
	#include "resources/icons/ico16_arrow_up.qgf.h"
	#include "resources/icons/ico16_arrow_down.qgf.h"
	#include "resources/icons/ico16_arrow_left.qgf.h"
	#include "resources/icons/ico16_arrow_right.qgf.h"
	// painter_image_handle_t lock_caps_on;
	// painter_image_handle_t lock_caps_off;
	// painter_image_handle_t lock_num_on;
	// painter_image_handle_t lock_num_off;
	// painter_image_handle_t lock_scrl_on;
	// painter_image_handle_t lock_scrl_off;
	painter_image_handle_t ico32_brightness;
	painter_image_handle_t ico16_arrow_up;
	painter_image_handle_t ico16_arrow_down;
	painter_image_handle_t ico16_arrow_left;
	painter_image_handle_t ico16_arrow_right;

void qp_init_load_files(void) {
	/* FONTS */
		thintel        = qp_load_font_mem(font_thintel15);
		roboto20       = qp_load_font_mem(font_roboto20);
		roboto25       = qp_load_font_mem(font_roboto25);
		robotobold25   = qp_load_font_mem(font_robotobold25);

	/* IMAGES & GIFS */
		// img_pikachu    = qp_load_image_mem(gfx_pikachu);
		// img_anya01     = qp_load_image_mem(gfx_anya01);
		// gif_pusheen    = qp_load_image_mem(gfx_pusheen_240px);
		// gif_nyan120px  = qp_load_image_mem(gfx_gif_nyan120px);
		// gif_cat01      = qp_load_image_mem(gfx_gif_cat01);
		// gif_cat02      = qp_load_image_mem(gfx_gif_cat02);
		// gif_dog01      = qp_load_image_mem(gfx_gif_dog01);
	
	/* ICONS */
		// lock_caps_on     = qp_load_image_mem(gfx_lock_caps_ON);
		// lock_caps_off    = qp_load_image_mem(gfx_lock_caps_OFF);
		// lock_num_on      = qp_load_image_mem(gfx_lock_num_ON);
		// lock_num_off     = qp_load_image_mem(gfx_lock_num_OFF);
		// lock_scrl_on     = qp_load_image_mem(gfx_lock_scrl_ON);
		// lock_scrl_off    = qp_load_image_mem(gfx_lock_scrl_OFF);
		ico32_brightness  = qp_load_image_mem(gfx_ico32_brightness);
		ico16_arrow_up    = qp_load_image_mem(gfx_ico16_arrow_up);
		ico16_arrow_down  = qp_load_image_mem(gfx_ico16_arrow_down);
		ico16_arrow_left  = qp_load_image_mem(gfx_ico16_arrow_left);
		ico16_arrow_right = qp_load_image_mem(gfx_ico16_arrow_right);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
