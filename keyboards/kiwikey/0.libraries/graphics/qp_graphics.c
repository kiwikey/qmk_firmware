#include QMK_KEYBOARD_H

#include <printf.h>
#include "qp.h"
#include "qp_graphics.h"
#include "qp_matrix_reactive.h"

#include "resources/thintel15.qff.h"
#include "resources/thintel40.qff.h"
#include "resources/arial30.qff.h"
#include "resources/bg_240x240.qgf.h"
#include "resources/pikachu.qgf.h"
#include "resources/pusheen_240px.qgf.h"
#include "resources/anya01.qgf.h"

static painter_image_handle_t img_bg_240x240;
static painter_image_handle_t img_pikachu;
static painter_image_handle_t img_anya01;
static painter_image_handle_t gif_pusheen;
static painter_font_handle_t  thintel;
static painter_font_handle_t  thintel40;
static painter_font_handle_t  arial30;
//static deferred_token         my_anim;

#include "resources/icons/lock-caps-ON.qgf.h"
#include "resources/icons/lock-scrl-ON.qgf.h"
#include "resources/icons/lock-num-ON.qgf.h"
#include "resources/icons/lock-caps-OFF.qgf.h"
#include "resources/icons/lock-scrl-OFF.qgf.h"
#include "resources/icons/lock-num-OFF.qgf.h"
static painter_image_handle_t lock_caps_on;
static painter_image_handle_t lock_caps_off;
static painter_image_handle_t lock_num_on;
static painter_image_handle_t lock_num_off;
static painter_image_handle_t lock_scrl_on;
static painter_image_handle_t lock_scrl_off;

void keyboard_post_init_display(void) {
	img_bg_240x240 = qp_load_image_mem(gfx_bg_240x240);
    img_pikachu    = qp_load_image_mem(gfx_pikachu);
    img_anya01     = qp_load_image_mem(gfx_anya01);
    gif_pusheen    = qp_load_image_mem(gfx_pusheen_240px);
    thintel        = qp_load_font_mem(font_thintel15);
	thintel40      = qp_load_font_mem(font_thintel40);
    arial30        = qp_load_font_mem(font_arial30);
	
	lock_caps_on  = qp_load_image_mem(gfx_lock_caps_ON);
    lock_caps_off = qp_load_image_mem(gfx_lock_caps_OFF);
    lock_num_on   = qp_load_image_mem(gfx_lock_num_ON);
    lock_num_off  = qp_load_image_mem(gfx_lock_num_OFF);
    lock_scrl_on  = qp_load_image_mem(gfx_lock_scrl_ON);
    lock_scrl_off = qp_load_image_mem(gfx_lock_scrl_OFF);
		
	qp_drawimage(my_display, 0, 0, img_bg_240x240);
	
	// qp_rect(my_display, 12, 9, 102, 39, HSV_CYAN, true);
	// qp_rect(my_display, 114, 7, 234, 127, HSV_BLACK, true);
	
	qp_drawtext_recolor(my_display, 15, 12, thintel40, "LAYER", 0, 0, 0, 0, 0, 255);
	render_solidmatrix_init();
}

void render_update_ui(void) {
	/** Current layer **/
	// char buf1[10] = {0};
    // sprintf(buf1, "-%d-", get_highest_layer(layer_state));
	// qp_drawtext(my_display, 90, 12, arial30, buf1);
	
	uint16_t curr_hue = 100;

	static led_t last_led_state = {0};
	last_led_state.raw = host_keyboard_led_state().raw;
	qp_drawimage_recolor(my_display, 12, 94, last_led_state.caps_lock ? lock_caps_on : lock_caps_off, curr_hue, 255, last_led_state.caps_lock ? 255 : 32, curr_hue, 255, 0);
	qp_drawimage_recolor(my_display, 12+32, 94, last_led_state.num_lock ? lock_num_on : lock_num_off, curr_hue, 255, last_led_state.num_lock ? 255 : 32, curr_hue, 255, 0);
	qp_drawimage_recolor(my_display, 12+32+32, 94, last_led_state.scroll_lock ? lock_scrl_on : lock_scrl_off, curr_hue, 255, last_led_state.scroll_lock ? 255 : 32, curr_hue, 255, 0);
}

bool process_record_display(uint16_t keycode, keyrecord_t *record) {
	render_solidmatrix_update(record->event.key.col, record->event.key.row);

	return true;
}

// void render_test01(void) {
    // qp_drawimage(my_display, 0, 0, img_pikachu);
    // static const char *text = "L";
    // int16_t width = qp_textwidth(arial30, text);
    // qp_drawtext_recolor(my_display, (239 - width), 0, arial30, text, 0, 0, 0,255, 255, 255);
// }
// void render_test02(void) {
    // my_anim = qp_animate(my_display, 0, 0, gif_pusheen);
    // qp_drawimage(my_display, 0, 0, img_anya01);    
    // static const char *text = "R";
    // int16_t width = qp_textwidth(arial30, text);
    // qp_drawtext_recolor(my_display, (239 - width), 0, arial30, text, 0, 0, 0,255, 255, 255);
// }
// void render_test03(void) {
	// qp_rect(my_display, 100, 100, 200, 200, HSV_CYAN, true);
	// static const char *text = "Hello from QMK!";
	// qp_drawtext(my_display, 50, 50,  thintel, text);
	// qp_drawtext(my_display, 50, 100, arial30, text);
// }