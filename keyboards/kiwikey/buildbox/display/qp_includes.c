#if defined(QUANTUM_PAINTER_ENABLE)

#include <qp.h>
#include "qp_includes.h"

/* FONTS */
	#include "resources/fonts/thintel16.qff.h"
	#include "resources/fonts/thintel32.qff.h"
	#include "resources/fonts/font_oled.qff.h"
	#include "resources/fonts/nanoplex16.qff.h"
	#include "resources/fonts/nanoplex32.qff.h"
	#include "resources/fonts/font16.qff.h"
	painter_font_handle_t  thintel16;
	painter_font_handle_t  thintel32;
	painter_font_handle_t  font_oled;
	painter_font_handle_t  nanoplex16;
	painter_font_handle_t  nanoplex32;
	painter_font_handle_t  font16;

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
	#include "resources/icons/ico12_arrow_left.qgf.h"
	#include "resources/icons/ico12_arrow_right.qgf.h"
	#include "resources/icons/ico22_gear.qgf.h"
	#include "resources/icons/ico16_layer.qgf.h"
	#include "resources/icons/ico32_menu.qgf.h"
	#include "resources/graphics/ico18_heart.qgf.h"

	// 22x22 batch (display/resources/icons/22x22/)
	#include "resources/icons/22x22/25.qgf.h"
	#include "resources/icons/22x22/47.qgf.h"
	#include "resources/icons/22x22/68.qgf.h"
	#include "resources/icons/22x22/72.qgf.h"
	#include "resources/icons/22x22/74.qgf.h"
	#include "resources/icons/22x22/86.qgf.h"
	#include "resources/icons/22x22/88.qgf.h"
	#include "resources/icons/22x22/Application.qgf.h"
	#include "resources/icons/22x22/Boss.qgf.h"
	#include "resources/icons/22x22/Calculator.qgf.h"
	#include "resources/icons/22x22/color_wheel.qgf.h"
	#include "resources/icons/22x22/Component.qgf.h"
	#include "resources/icons/22x22/Desktop.qgf.h"
	#include "resources/icons/22x22/Earth.qgf.h"
	#include "resources/icons/22x22/Favourites.qgf.h"
	#include "resources/icons/22x22/Film.qgf.h"
	#include "resources/icons/22x22/game_controller.qgf.h"
	#include "resources/icons/22x22/Gear.qgf.h"
	#include "resources/icons/22x22/Globe.qgf.h"
	#include "resources/icons/22x22/Heart.qgf.h"
	#include "resources/icons/22x22/Heart1.qgf.h"
	#include "resources/icons/22x22/Mail.qgf.h"
	#include "resources/icons/22x22/Mouse.qgf.h"
	#include "resources/icons/22x22/Online.qgf.h"
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
	painter_image_handle_t ico12_arrow_left;
	painter_image_handle_t ico12_arrow_right;
	painter_image_handle_t ico22_gear;
	painter_image_handle_t ico18_heart;

	painter_image_handle_t ico22_25;
	painter_image_handle_t ico22_47;
	painter_image_handle_t ico22_68;
	painter_image_handle_t ico22_72;
	painter_image_handle_t ico22_74;
	painter_image_handle_t ico22_86;
	painter_image_handle_t ico22_88;
	painter_image_handle_t ico22_application;
	painter_image_handle_t ico22_boss;
	painter_image_handle_t ico22_calculator;
	painter_image_handle_t ico22_color_wheel;
	painter_image_handle_t ico22_component;
	painter_image_handle_t ico22_desktop;
	painter_image_handle_t ico22_earth;
	painter_image_handle_t ico22_favourites;
	painter_image_handle_t ico22_film;
	painter_image_handle_t ico22_game_controller;
	painter_image_handle_t ico22_gear2;
	painter_image_handle_t ico22_globe;
	painter_image_handle_t ico22_heart;
	painter_image_handle_t ico22_heart1;
	painter_image_handle_t ico22_mail;
	painter_image_handle_t ico22_mouse;
	painter_image_handle_t ico22_online;

/* IMAGES & ANIMATIONS */
	#include "resources/graphics/gif_bootup01.qgf.h"
	// #include "resources/pusheen_240px.qgf.h"
	#include "resources/graphics/anya01.qgf.h"
	#include "resources/graphics/gif_nyan120px.qgf.h"
	#include "resources/graphics/gif_cat01.qgf.h"
	// #include "resources/gif_cat02.qgf.h"
	// #include "resources/gif_dog01.qgf.h"
	#include "resources/graphics/knob.qgf.h"
	painter_image_handle_t gif_bootup01;
	painter_image_handle_t img_anya01;
	// painter_image_handle_t gif_pusheen;
	painter_image_handle_t gif_nyan120px;
	painter_image_handle_t gif_cat01;
	// painter_image_handle_t gif_cat02;
	// painter_image_handle_t gif_dog01;
	painter_image_handle_t img_knob;
	deferred_token         my_anim;

void qp_init_load_files(void) {
	/* FONTS */
		thintel16         = qp_load_font_mem(font_thintel16);
		thintel32         = qp_load_font_mem(font_thintel32);
		font_oled         = qp_load_font_mem(font_oled_font);
		nanoplex16        = qp_load_font_mem(font_nanoplex16);
		nanoplex32        = qp_load_font_mem(font_nanoplex32);
		font16            = qp_load_font_mem(font_font16);

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
		ico12_arrow_left  = qp_load_image_mem(gfx_ico12_arrow_left);
		ico12_arrow_right = qp_load_image_mem(gfx_ico12_arrow_right);
		ico22_gear        = qp_load_image_mem(gfx_ico22_gear);
		ico18_heart       = qp_load_image_mem(gfx_ico18_heart);

		ico22_25              = qp_load_image_mem(gfx_25);
		ico22_47              = qp_load_image_mem(gfx_47);
		ico22_68              = qp_load_image_mem(gfx_68);
		ico22_72              = qp_load_image_mem(gfx_72);
		ico22_74              = qp_load_image_mem(gfx_74);
		ico22_86              = qp_load_image_mem(gfx_86);
		ico22_88              = qp_load_image_mem(gfx_88);
		ico22_application     = qp_load_image_mem(gfx_Application);
		ico22_boss            = qp_load_image_mem(gfx_Boss);
		ico22_calculator      = qp_load_image_mem(gfx_Calculator);
		ico22_color_wheel     = qp_load_image_mem(gfx_Color_wheel);
		ico22_component       = qp_load_image_mem(gfx_Component);
		ico22_desktop         = qp_load_image_mem(gfx_Desktop);
		ico22_earth           = qp_load_image_mem(gfx_Earth);
		ico22_favourites      = qp_load_image_mem(gfx_Favourites);
		ico22_film            = qp_load_image_mem(gfx_Film);
		ico22_game_controller = qp_load_image_mem(gfx_Game_controller);
		ico22_gear2           = qp_load_image_mem(gfx_Gear);
		ico22_globe           = qp_load_image_mem(gfx_Globe);
		ico22_heart           = qp_load_image_mem(gfx_Heart);
		ico22_heart1          = qp_load_image_mem(gfx_Heart1);
		ico22_mail            = qp_load_image_mem(gfx_Mail);
		ico22_mouse           = qp_load_image_mem(gfx_Mouse);
		ico22_online          = qp_load_image_mem(gfx_Online);

	/* IMAGES & ANIMATIONS */
		gif_bootup01      = qp_load_image_mem(gfx_gif_bootup01);
		img_anya01        = qp_load_image_mem(gfx_anya01);
		// gif_pusheen    = qp_load_image_mem(gfx_pusheen_240px);
		gif_nyan120px     = qp_load_image_mem(gfx_gif_nyan120px);
		gif_cat01         = qp_load_image_mem(gfx_gif_cat01);
		// gif_cat02      = qp_load_image_mem(gfx_gif_cat02);
		// gif_dog01      = qp_load_image_mem(gfx_gif_dog01);
		img_knob          = qp_load_image_mem(gfx_knob);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
