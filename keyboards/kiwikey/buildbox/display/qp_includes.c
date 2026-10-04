#if defined(QUANTUM_PAINTER_ENABLE)

#include <qp.h>
#include "debug.h" // dprintf()
#include "qp_includes.h"

/* FONTS */
	#include "resources/fonts/thintel32.qff.h"
	#include "resources/fonts/font_oled.qff.h"
	#include "resources/fonts/nanoplex16.qff.h"
	#include "resources/fonts/nanoplex32.qff.h"
	#include "resources/fonts/font16.qff.h"
	painter_font_handle_t  thintel32;
	painter_font_handle_t  font_oled;
	painter_font_handle_t  nanoplex16;
	painter_font_handle_t  nanoplex32;
	painter_font_handle_t  font16;

/* ICONS */
	#include "resources/icons/ico32_brightness.qgf.h"
	#include "resources/icons/ico16_arrow_up.qgf.h"
	#include "resources/icons/ico16_arrow_down.qgf.h"
	#include "resources/icons/ico16_arrow_left.qgf.h"
	#include "resources/icons/ico16_arrow_right.qgf.h"
	#include "resources/icons/ico12_arrow_left.qgf.h"
	#include "resources/icons/ico12_arrow_right.qgf.h"
	#include "resources/icons/ico22_gear.qgf.h"

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
	painter_image_handle_t ico32_brightness;
	painter_image_handle_t ico16_arrow_up;
	painter_image_handle_t ico16_arrow_down;
	painter_image_handle_t ico16_arrow_left;
	painter_image_handle_t ico16_arrow_right;
	painter_image_handle_t ico12_arrow_left;
	painter_image_handle_t ico12_arrow_right;
	painter_image_handle_t ico22_gear;

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
	#include "resources/graphics/knob.qgf.h"
	painter_image_handle_t gif_bootup01;
	painter_image_handle_t img_knob;
	deferred_token         bb_boot_anim;

// Every font/image is loaded once here and kept resident. qp_load_*_mem() returns
// NULL when the data is invalid or QP's fixed pool is full (QUANTUM_PAINTER_NUM_FONTS/
// QUANTUM_PAINTER_NUM_IMAGES in config.h) - log which one, and count it, instead of
// failing silently. QP's own draw calls ignore a NULL handle, but code that reads
// ->width/->height/->line_height directly must check it (see qp_all_files_loaded()).
static uint8_t qp_load_failures = 0;

#define QP_LOAD_FONT(handle, data) do { \
		handle = qp_load_font_mem(data); \
		if (!handle) { qp_load_failures++; dprintf("qp_init_load_files: font " #data " failed to load\n"); } \
	} while (0)

#define QP_LOAD_IMAGE(handle, data) do { \
		handle = qp_load_image_mem(data); \
		if (!handle) { qp_load_failures++; dprintf("qp_init_load_files: image " #data " failed to load - QUANTUM_PAINTER_NUM_IMAGES too small?\n"); } \
	} while (0)

bool qp_all_files_loaded(void) {
	return qp_load_failures == 0;
}

void qp_init_load_files(void) {
	qp_load_failures = 0;
	/* FONTS */
		QP_LOAD_FONT(thintel32,         font_thintel32);
		QP_LOAD_FONT(font_oled,         font_oled_font);
		QP_LOAD_FONT(nanoplex16,        font_nanoplex16);
		QP_LOAD_FONT(nanoplex32,        font_nanoplex32);
		QP_LOAD_FONT(font16,            font_font16);

	/* ICONS */
		QP_LOAD_IMAGE(ico32_brightness,  gfx_ico32_brightness);
		QP_LOAD_IMAGE(ico16_arrow_up,    gfx_ico16_arrow_up);
		QP_LOAD_IMAGE(ico16_arrow_down,  gfx_ico16_arrow_down);
		QP_LOAD_IMAGE(ico16_arrow_left,  gfx_ico16_arrow_left);
		QP_LOAD_IMAGE(ico16_arrow_right, gfx_ico16_arrow_right);
		QP_LOAD_IMAGE(ico12_arrow_left,  gfx_ico12_arrow_left);
		QP_LOAD_IMAGE(ico12_arrow_right, gfx_ico12_arrow_right);
		QP_LOAD_IMAGE(ico22_gear,        gfx_ico22_gear);

		QP_LOAD_IMAGE(ico22_25,              gfx_25);
		QP_LOAD_IMAGE(ico22_47,              gfx_47);
		QP_LOAD_IMAGE(ico22_68,              gfx_68);
		QP_LOAD_IMAGE(ico22_72,              gfx_72);
		QP_LOAD_IMAGE(ico22_74,              gfx_74);
		QP_LOAD_IMAGE(ico22_86,              gfx_86);
		QP_LOAD_IMAGE(ico22_88,              gfx_88);
		QP_LOAD_IMAGE(ico22_application,     gfx_Application);
		QP_LOAD_IMAGE(ico22_boss,            gfx_Boss);
		QP_LOAD_IMAGE(ico22_calculator,      gfx_Calculator);
		QP_LOAD_IMAGE(ico22_color_wheel,     gfx_Color_wheel);
		QP_LOAD_IMAGE(ico22_component,       gfx_Component);
		QP_LOAD_IMAGE(ico22_desktop,         gfx_Desktop);
		QP_LOAD_IMAGE(ico22_earth,           gfx_Earth);
		QP_LOAD_IMAGE(ico22_favourites,      gfx_Favourites);
		QP_LOAD_IMAGE(ico22_film,            gfx_Film);
		QP_LOAD_IMAGE(ico22_game_controller, gfx_Game_controller);
		QP_LOAD_IMAGE(ico22_gear2,           gfx_Gear);
		QP_LOAD_IMAGE(ico22_globe,           gfx_Globe);
		QP_LOAD_IMAGE(ico22_heart,           gfx_Heart);
		QP_LOAD_IMAGE(ico22_heart1,          gfx_Heart1);
		QP_LOAD_IMAGE(ico22_mail,            gfx_Mail);
		QP_LOAD_IMAGE(ico22_mouse,           gfx_Mouse);
		QP_LOAD_IMAGE(ico22_online,          gfx_Online);

	/* IMAGES & ANIMATIONS */
		// gif_bootup01 is NOT loaded here - keyboard_post_init_display() (qp_graphics.c) loads it on demand and closes it after the boot animation
		QP_LOAD_IMAGE(img_knob,          gfx_knob);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
