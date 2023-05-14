#include QMK_KEYBOARD_H

#include "qp.h"

#include "sport48.h"
#include "sport48_graphics.h"

#include "thintel15.qff.h"
#include "arial30.qff.h"
#include "pikachu.qgf.h"
#include "anya01.qgf.h"
#include "pusheen_240px.qgf.h"

static painter_image_handle_t img_pikachu;
static painter_image_handle_t img_anya01;
static painter_image_handle_t gif_pusheen;
static painter_font_handle_t  thintel;
static painter_font_handle_t  arial30;
//static deferred_token         my_anim;

//----------------------------------------------------------
// UI Initialisation
void draw_ui_user(void) {
	qp_drawimage(my_display, 0, 0, img_pikachu);
	
	// static const char *text = "MASTER (left)";
    // int16_t width = qp_textwidth(arial30, text);
	// qp_drawtext(my_display, (239 - width), (239 - arial30->line_height), arial30, text);

	static const char *text = "Le Tri Thong";
   // int16_t width = qp_textwidth(arial30, text);
	qp_drawtext(my_display, 0, 0, arial30, text);
	
	static const char *text2 = " 13.05.2023 ";
    //int16_t width2 = qp_textwidth(arial30, text2);
	qp_drawtext(my_display, 0, arial30->line_height, arial30, text2);
	
		static const char *text3 = "#VNMK";
    //int16_t width2 = qp_textwidth(arial30, text2);
	qp_drawtext(my_display, 0, (arial30->line_height)*2, arial30, text3);
}
void draw_right_display(void) {
	//my_anim = qp_animate(my_display, 0, 0, gif_pusheen);

	qp_drawimage(my_display, 0, 0, img_anya01);	
	static const char *text = "SLAVE (right)";
	int16_t width = qp_textwidth(arial30, text);
	qp_drawtext(my_display, (239 - width), (239 - arial30->line_height), arial30, text);
}

void keyboard_post_init_display(void) {
	img_pikachu = qp_load_image_mem(gfx_pikachu);
	img_anya01  = qp_load_image_mem(gfx_anya01);
    gif_pusheen = qp_load_image_mem(gfx_pusheen_240px);
	thintel     = qp_load_font_mem(font_thintel15);
	arial30     = qp_load_font_mem(font_arial30);
	
	//int16_t width = qp_textwidth(arial30, text);
	//qp_drawtext(my_display, (239 - width), (239 - arial30->line_height), arial30, text);
	
	if (is_keyboard_left()) {
        draw_ui_user();
    } else {
        draw_right_display();
    }
}
