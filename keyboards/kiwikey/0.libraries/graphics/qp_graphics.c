#include QMK_KEYBOARD_H

#include <printf.h>
#include "qp.h"
#include "qp_graphics.h"
#include "qp_matrix_reactive.h"

#include "resources/thintel15.qff.h"
#include "resources/arial30.qff.h"
#include "resources/pikachu.qgf.h"
#include "resources/anya01.qgf.h"
#include "resources/pusheen_240px.qgf.h"

static painter_image_handle_t img_pikachu;
static painter_image_handle_t img_anya01;
static painter_image_handle_t gif_pusheen;
static painter_font_handle_t  thintel;
static painter_font_handle_t  arial30;
//static deferred_token         my_anim;

void keyboard_post_init_display(void) {
    img_pikachu = qp_load_image_mem(gfx_pikachu);
    img_anya01  = qp_load_image_mem(gfx_anya01);
    gif_pusheen = qp_load_image_mem(gfx_pusheen_240px);
    thintel     = qp_load_font_mem(font_thintel15);
    arial30     = qp_load_font_mem(font_arial30);
	
	// render_solidmatrix_init();
	qp_drawimage(my_display, 0, 0, img_pikachu);
}

void render_ui_user(void) {
	/** Current layer **/
	char buf1[32] = {0};
    sprintf(buf1, "Layer %d", get_highest_layer(layer_state));
	qp_drawtext(my_display, 0, 0, arial30, buf1);
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