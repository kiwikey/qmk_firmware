#if defined(QUANTUM_PAINTER_ENABLE)

#include <qp.h>
#include "qp_includes.h"

/* FONTS */
	#include "resources/fonts/thintel15.qff.h"
	#include "resources/fonts/roboto20.qff.h"
	#include "resources/fonts/roboto25.qff.h"
	#include "resources/fonts/robotobold25.qff.h"
	#include "resources/fonts/font_oled.qff.h"
	#include "resources/fonts/font_proggy_clean.qff.h"
	#include "resources/fonts/font_proggy_tiny.qff.h"
	painter_font_handle_t  thintel15;
	painter_font_handle_t  roboto20;
	painter_font_handle_t  roboto25;
	painter_font_handle_t  robotobold25;
	painter_font_handle_t  font_oled;
	painter_font_handle_t  font_proggy_clean;
	painter_font_handle_t  font_proggy_tiny;

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
	#include "resources/icons/ico16_layer.qgf.h"
	#include "resources/icons/ico32_menu.qgf.h"
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
	painter_image_handle_t ico16_layer;
	painter_image_handle_t ico32_menu;

/* IMAGES & ANIMATIONS */
	#include "resources/graphics/gif_bootup01.qgf.h"
	// #include "resources/pusheen_240px.qgf.h"
	#include "resources/graphics/anya01.qgf.h"
	#include "resources/graphics/gif_nyan120px.qgf.h"
	#include "resources/graphics/gif_cat01.qgf.h"
	// #include "resources/gif_cat02.qgf.h"
	// #include "resources/gif_dog01.qgf.h"
	painter_image_handle_t gif_bootup01;
	painter_image_handle_t img_anya01;
	// painter_image_handle_t gif_pusheen;
	painter_image_handle_t gif_nyan120px;
	painter_image_handle_t gif_cat01;
	// painter_image_handle_t gif_cat02;
	// painter_image_handle_t gif_dog01;
	deferred_token         my_anim;

void qp_init_load_files(void) {
	/* FONTS */
		thintel15      = qp_load_font_mem(font_thintel15);
		roboto20          = qp_load_font_mem(font_roboto20);
		roboto25          = qp_load_font_mem(font_roboto25);
		robotobold25      = qp_load_font_mem(font_robotobold25);
		font_oled         = qp_load_font_mem(font_oled_font);
		font_proggy_clean = qp_load_font_mem(font_proggy_clean_15);
		font_proggy_tiny  = qp_load_font_mem(font_ProggyTiny15);

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
		ico16_layer       = qp_load_image_mem(gfx_ico16_layer);
		ico32_menu        = qp_load_image_mem(gfx_ico32_menu);

	/* IMAGES & ANIMATIONS */
		gif_bootup01      = qp_load_image_mem(gfx_gif_bootup01);
		img_anya01        = qp_load_image_mem(gfx_anya01);
		// gif_pusheen    = qp_load_image_mem(gfx_pusheen_240px);
		gif_nyan120px     = qp_load_image_mem(gfx_gif_nyan120px);
		gif_cat01         = qp_load_image_mem(gif_cat01);
		// gif_cat02      = qp_load_image_mem(gfx_gif_cat02);
		// gif_dog01      = qp_load_image_mem(gfx_gif_dog01);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
