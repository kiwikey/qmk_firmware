#include QMK_KEYBOARD_H

#include <printf.h>
#include "qp.h"
#include "qp_graphics.h"
#include "qp_matrix_reactive.h"

#include "resources/thintel15.qff.h"
#include "resources/thintel50.qff.h"
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
static painter_font_handle_t  thintel50;
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

void qp_roundrect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint8_t hue, uint8_t sat, uint8_t val, bool filled, uint16_t corner, bool roundtop, bool roundbottom) {	
	qp_rect(my_display, left, top+corner, right, bottom-corner, hue, sat, val, filled);
	if (roundtop) {
		qp_rect(my_display, left+corner, top, right-corner, bottom, hue, sat, val, filled);
		qp_circle(my_display, left+corner, top+corner, corner, hue, sat, val, filled);
		qp_circle(my_display, right-corner, top+corner, corner, hue, sat, val, filled);
	} else {
		qp_rect(my_display, left, top, right, top+corner, hue, sat, val, filled);
	}
	if (roundbottom) {
		qp_rect(my_display, left+corner, bottom-corner, right-corner, bottom, hue, sat, val, filled);
		qp_circle(my_display, left+corner, bottom-corner, corner, hue, sat, val, filled);
		qp_circle(my_display, right-corner, bottom-corner, corner, hue, sat, val, filled);
	} else {
		qp_rect(my_display, left, bottom-corner, right, bottom, hue, sat, val, filled);
	}
}

uint8_t layer_color_hue[4] = { 128, 213, 43, 85 };

void keyboard_post_init_display(void) {
	img_bg_240x240 = qp_load_image_mem(gfx_bg_240x240);
    img_pikachu    = qp_load_image_mem(gfx_pikachu);
    img_anya01     = qp_load_image_mem(gfx_anya01);
    gif_pusheen    = qp_load_image_mem(gfx_pusheen_240px);
    thintel        = qp_load_font_mem(font_thintel15);
	thintel50      = qp_load_font_mem(font_thintel50);
    arial30        = qp_load_font_mem(font_arial30);
	
	lock_caps_on  = qp_load_image_mem(gfx_lock_caps_ON);
    lock_caps_off = qp_load_image_mem(gfx_lock_caps_OFF);
    lock_num_on   = qp_load_image_mem(gfx_lock_num_ON);
    lock_num_off  = qp_load_image_mem(gfx_lock_num_OFF);
    lock_scrl_on  = qp_load_image_mem(gfx_lock_scrl_ON);
    lock_scrl_off = qp_load_image_mem(gfx_lock_scrl_OFF);
		
	qp_drawimage(my_display, 0, 0, img_pikachu);
	
	// Widget Layers
	#define UI_LAYER_X      5
	#define UI_LAYER_Y      5
	#define UI_LAYER_SIZE   108
	#define UI_LAYER_TEXTCOLOR  HSV_BLACK
	#define UI_SHADOW_POS		3
	
	qp_roundrect(UI_LAYER_X, UI_LAYER_Y, UI_LAYER_X+UI_LAYER_SIZE, UI_LAYER_Y+UI_LAYER_SIZE/3, HSV_WHITE, true, 5, true, false);
	qp_drawtext_recolor(my_display, 25, 9, thintel50, "LAYER", UI_LAYER_TEXTCOLOR, HSV_WHITE);
	for (uint8_t i = 0; i < 4; i++) {
		qp_rect(my_display,
				UI_LAYER_X+i*UI_LAYER_SIZE/4,
				UI_LAYER_Y+(UI_LAYER_SIZE/3)+1,
				UI_LAYER_X+UI_LAYER_SIZE/4*(i+1),
				UI_LAYER_Y+UI_LAYER_SIZE*2/3,
				layer_color_hue[i], 255, 255, true);
	}
	qp_roundrect(UI_LAYER_X,
				UI_LAYER_Y+UI_LAYER_SIZE*2/3,
				UI_LAYER_X+UI_LAYER_SIZE,
				UI_LAYER_Y+UI_LAYER_SIZE,
				layer_color_hue[0], 255, 255, true, 5, false, true);

	// Widget Matrix reactive
	#define MATRIX_REACTIVE_UI_BORDER 7
	qp_roundrect(MATRIX_REACTIVE_DISPLAY_X-MATRIX_REACTIVE_UI_BORDER+UI_SHADOW_POS,
				 MATRIX_REACTIVE_DISPLAY_Y-MATRIX_REACTIVE_UI_BORDER+1+UI_SHADOW_POS,
				 MATRIX_REACTIVE_DISPLAY_X+95+MATRIX_REACTIVE_UI_BORDER+UI_SHADOW_POS,
				 MATRIX_REACTIVE_DISPLAY_Y+95+MATRIX_REACTIVE_UI_BORDER+UI_SHADOW_POS,
				 0,0,100, true, 5, true, true);
	qp_roundrect(MATRIX_REACTIVE_DISPLAY_X-MATRIX_REACTIVE_UI_BORDER,
				 MATRIX_REACTIVE_DISPLAY_Y-MATRIX_REACTIVE_UI_BORDER+1,
				 MATRIX_REACTIVE_DISPLAY_X+95+MATRIX_REACTIVE_UI_BORDER,
				 MATRIX_REACTIVE_DISPLAY_Y+95+MATRIX_REACTIVE_UI_BORDER,
				 HSV_BLACK, true, 5, true, true);
	
	// qp_rect(my_display, UI_LAYER_BG_X, UI_LAYER_BG_Y, UI_LAYER_BG_X+108, UI_LAYER_BG_Y+36, UI_LAYER_COLOR0, true);
	// qp_drawtext_recolor(my_display, 13, 44+36, arial30, "0", UI_LAYER_TEXTCOLOR, HSV_CYAN);
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
	qp_drawimage_recolor(my_display, 12, 120, last_led_state.caps_lock ? lock_caps_on : lock_caps_off, curr_hue, 255, last_led_state.caps_lock ? 255 : 32, curr_hue, 255, 0);
	qp_drawimage_recolor(my_display, 12+32, 120, last_led_state.num_lock ? lock_num_on : lock_num_off, curr_hue, 255, last_led_state.num_lock ? 255 : 32, curr_hue, 255, 0);
	qp_drawimage_recolor(my_display, 12+32+32, 120, last_led_state.scroll_lock ? lock_scrl_on : lock_scrl_off, curr_hue, 255, last_led_state.scroll_lock ? 255 : 32, curr_hue, 255, 0);
}

bool process_record_display(uint16_t keycode, keyrecord_t *record) {
	render_solidmatrix_update(record->event.key.col, record->event.key.row);

	return true;
}

layer_state_t layer_state_set_display(layer_state_t state) {
	uint8_t tmp_layer = get_highest_layer(state);
	for (uint8_t i = 0; i < 4; i++) {
		char buf1[10] = {0};
		sprintf(buf1, "%d", i);
		qp_drawtext_recolor(my_display, 12+i*27, 45,    // Write layer number
					thintel50, buf1,
					0, 0, (tmp_layer == i) ? 0 : 200,
					layer_color_hue[i], 255, 255);
	}
	qp_roundrect(UI_LAYER_X,    // Draw layer_name's background
				UI_LAYER_Y+UI_LAYER_SIZE*2/3,
				UI_LAYER_X+UI_LAYER_SIZE,
				UI_LAYER_Y+UI_LAYER_SIZE,
				layer_color_hue[tmp_layer], 255, 255, true, 5, false, true);
	// Write layer_name
	qp_drawtext_recolor(my_display, 15, 79, thintel50, layer_name[tmp_layer], UI_LAYER_TEXTCOLOR, layer_color_hue[tmp_layer], 255, 255);
	
	return state;
}