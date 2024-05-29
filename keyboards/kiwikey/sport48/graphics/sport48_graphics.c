#include QMK_KEYBOARD_H

#include <printf.h>
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

/*** KEYMAP MATRIX ***/
#define MATRIX_DISPLAY_X    20
#define MATRIX_DISPLAY_Y    90
#define KEY_WIDTH           25
#define KEY_HEIGHT          25
#define KEY_SPACING         10

char buf[20] = {0};

const char code_to_name[60] = {
    // ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f',
    // 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    // 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    // '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    // 'R', 'E', 'B', 'T', ' ', ' ', ' ', ' ', ' ', ' ',
    // ' ', ';', '\'', ' ', ',', '.', '/', ' ', ' ', ' '
	
	' ', ' ', ' ', ' ', 'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'R', 'E', 'B', 'T', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ';', '\'', ' ', ',', '.', '/', ' ', ' ', ' '
};

void render_matrix_left(void) {
    for (uint8_t x = 0; x < MATRIX_ROWS/2; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS-1; y++) {
            bool on = (matrix_get_row(x) & (1 << y)) > 0; // The matrix position [x,y] is being pressed
            // qp_rect(my_display,
                    // MATRIX_DISPLAY_X -6 + y*(KEY_WIDTH  + KEY_SPACING), // left
                    // MATRIX_DISPLAY_Y -6 + x*(KEY_HEIGHT + KEY_SPACING), // top
                    // MATRIX_DISPLAY_X +4 + y*(KEY_WIDTH  + KEY_SPACING) + KEY_WIDTH, // right
                    // MATRIX_DISPLAY_Y +4 + x*(KEY_HEIGHT + KEY_SPACING) + KEY_HEIGHT, // bottom
                    // 255, // hue
                    // (on ? 255 : 0), // sat
                    // 255, // val
                    // false);
			uint16_t keycode = keymap_key_to_keycode(0, (keypos_t){y,x});
			snprintf(buf, sizeof(buf), "%c", code_to_name[keycode]);
			qp_drawtext_recolor(my_display,
								MATRIX_DISPLAY_X + y*(KEY_WIDTH + KEY_SPACING),
								MATRIX_DISPLAY_Y + x*(KEY_HEIGHT + KEY_SPACING),
								arial30, buf,
								255, (on ? 255 : 0), 255, // text color (HUE, SAT, VAL)
								255, 0, 0 // background color (HUE, SAT, VAL)
								);
        }
    }
}

void render_matrix_right(void) {
    for (uint8_t x = MATRIX_ROWS/2; x < MATRIX_ROWS; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS-1; y++) {
            bool on = (matrix_get_row(x) & (1 << y)) > 0; // The matrix position [x,y] is being pressed
            // qp_rect(my_display,
                    // MATRIX_DISPLAY_X -8 + y*(KEY_WIDTH  + KEY_SPACING), // left
                    // MATRIX_DISPLAY_Y -5 + (x-4)*(KEY_HEIGHT + KEY_SPACING), // top
                    // MATRIX_DISPLAY_X + y*(KEY_WIDTH  + KEY_SPACING) + KEY_WIDTH, // right
                    // MATRIX_DISPLAY_Y + (x-4)*(KEY_HEIGHT + KEY_SPACING) + KEY_HEIGHT, // bottom
                    // 255, // hue
                    // (on ? 255 : 0), // sat
                    // 255, // val
                    // false);
			uint16_t keycode = keymap_key_to_keycode(0, (keypos_t){y,x});
			snprintf(buf, sizeof(buf), "%c", code_to_name[keycode]);
			qp_drawtext_recolor(my_display,
								MATRIX_DISPLAY_X + (5-y)*(KEY_WIDTH + KEY_SPACING),
								MATRIX_DISPLAY_Y + (x-4)*(KEY_HEIGHT + KEY_SPACING),
								arial30, buf,
								255, (on ? 255 : 0), 255, // text color (HUE, SAT, VAL)
								255, 0, 0 // background color (HUE, SAT, VAL)
								);
        }
    }
}

void render_matrix_left_update(uint8_t col, uint8_t row) {
	bool on = (matrix_get_row(row) & (1 << col)) > 0; // The matrix position [x,y] is being pressed
	uint16_t keycode = keymap_key_to_keycode(0, (keypos_t){col,row});
	snprintf(buf, sizeof(buf), "%c", code_to_name[keycode]);
	qp_drawtext_recolor(my_display,
						MATRIX_DISPLAY_X + col*(KEY_WIDTH + KEY_SPACING),
						MATRIX_DISPLAY_Y + row*(KEY_HEIGHT + KEY_SPACING),
						arial30, buf,
						255, (on ? 255 : 0), 255, // text color (HUE, SAT, VAL)
						255, 0, 0 // background color (HUE, SAT, VAL)
						);
}

void render_matrix_right_update(uint8_t col, uint8_t row) {
	bool on = (matrix_get_row(row) & (1 << col)) > 0; // The matrix position [x,y] is being pressed
	uint16_t keycode = keymap_key_to_keycode(0, (keypos_t){col,row});
	snprintf(buf, sizeof(buf), "%c", code_to_name[keycode]);
	qp_drawtext_recolor(my_display,
						MATRIX_DISPLAY_X + (5-col)*(KEY_WIDTH + KEY_SPACING),
						MATRIX_DISPLAY_Y + (row-4)*(KEY_HEIGHT + KEY_SPACING),
						arial30, buf,
						255, (on ? 255 : 0), 255, // text color (HUE, SAT, VAL)
						255, 0, 0 // background color (HUE, SAT, VAL)
						);
}
/*** END OF KEYMAP MATRIX ***/

/*** MATRIX SOLID REACTIVE (without KEYMAP) ***/
#define MATRIX_REACTIVE_DISPLAY_X    30
#define MATRIX_REACTIVE_DISPLAY_Y    120
#define MATRIX_REACTIVE_KEY_WIDTH    15
#define MATRIX_REACTIVE_KEY_HEIGHT   15
#define MATRIX_REACTIVE_KEY_SPACING  5

void render_solidmatrix_init(void) {
    for (uint8_t x = 0; x < MATRIX_ROWS/2; x++) {
        for (uint8_t y = 0; y < MATRIX_COLS-1; y++) {
            qp_rect(my_display,
                    MATRIX_REACTIVE_DISPLAY_X + y*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING), // left
                    MATRIX_REACTIVE_DISPLAY_Y + x*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING), // top
                    MATRIX_REACTIVE_DISPLAY_X + y*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_WIDTH, // right
                    MATRIX_REACTIVE_DISPLAY_Y + x*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_HEIGHT, // bottom
                    255, // hue
                    0, // sat
                    255, // val
                    true);
        }
    }
}

void render_solidmatrix_update(uint8_t col, uint8_t row) {
	char buf1[32] = {0};
    sprintf(buf1, "Row %d Col %d", row, col);
	qp_drawtext(my_display, 0, 30, arial30, buf1);
	
	bool on = (matrix_get_row(row) & (1 << col)) > 0; // The matrix position [x,y] is being pressed
	switch (row) {
		case 0 ... 3:
			if (!is_keyboard_left()) break;
			qp_rect(my_display,
					MATRIX_REACTIVE_DISPLAY_X + col*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING), // left
					MATRIX_REACTIVE_DISPLAY_Y + row*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING), // top
					MATRIX_REACTIVE_DISPLAY_X + col*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_WIDTH, // right
					MATRIX_REACTIVE_DISPLAY_Y + row*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_HEIGHT, // bottom
					255, // hue
					(on ? 255 : 0), // sat
					255, // val
					true);
			break;
		case 4 ... 7:
			if (is_keyboard_left()) break;
			qp_rect(my_display,
					MATRIX_REACTIVE_DISPLAY_X + (5-col)*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING), // left
					MATRIX_REACTIVE_DISPLAY_Y + (row-4)*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING), // top
					MATRIX_REACTIVE_DISPLAY_X + (5-col)*(MATRIX_REACTIVE_KEY_WIDTH  + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_WIDTH, // right
					MATRIX_REACTIVE_DISPLAY_Y + (row-4)*(MATRIX_REACTIVE_KEY_HEIGHT + MATRIX_REACTIVE_KEY_SPACING) + MATRIX_REACTIVE_KEY_HEIGHT, // bottom
					255, // hue
					(on ? 255 : 0), // sat
					255, // val
					true);
			break;
	}
}






/*** END OF MATRIX SOLID REACTIVE ***/

//----------------------------------------------------------
// UI Initialisation
void render_ui(void) {
	// Current layer
	char buf1[32] = {0};
    sprintf(buf1, "Layer %d", get_highest_layer(layer_state));
	qp_drawtext(my_display, 0, 0, arial30, buf1);
}

void render_test01(void) {
    qp_drawimage(my_display, 0, 0, img_pikachu);
    static const char *text = "L";
    int16_t width = qp_textwidth(arial30, text);
    qp_drawtext_recolor(my_display, (239 - width), 0, arial30, text, 0, 0, 0,255, 255, 255);
}
void render_test02(void) {
    //my_anim = qp_animate(my_display, 0, 0, gif_pusheen);
    qp_drawimage(my_display, 0, 0, img_anya01);    
    static const char *text = "R";
    int16_t width = qp_textwidth(arial30, text);
    qp_drawtext_recolor(my_display, (239 - width), 0, arial30, text, 0, 0, 0,255, 255, 255);
}

void keyboard_post_init_display(void) {
    img_pikachu = qp_load_image_mem(gfx_pikachu);
    img_anya01  = qp_load_image_mem(gfx_anya01);
    gif_pusheen = qp_load_image_mem(gfx_pusheen_240px);
    thintel     = qp_load_font_mem(font_thintel15);
    arial30     = qp_load_font_mem(font_arial30);
    
    //int16_t width = qp_textwidth(arial30, text);
    //qp_drawtext(my_display, (239 - width), (239 - arial30->line_height), arial30, text);
    
    render_solidmatrix_init();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    // switch (keycode) {
        // case QK_LIGHTING ... QK_LIGHTING_MAX:
            // ui_mode = 1;
            // ui_clear();
            // break;
        // case KC_AUDIO_MUTE ... KC_MEDIA_EJECT:
            // ui_mode = 2;
            // ui_clear();
            // break;
        // default:
            // break;
    // }
	if (is_keyboard_left()) {
        SEND_STRING("left process_record_kb");
    } else {
        SEND_STRING("right process_record_kb");
    }
	char buf1[32] = {0};
    sprintf(buf1, "Test %d", get_highest_layer(layer_state));
	qp_drawtext(my_display, 0, 60, arial30, buf1);
	
	render_solidmatrix_update(record->event.key.col, record->event.key.row);
	return true; // Process all other keycodes normally
}

void housekeeping_task_kb(void) {
	render_ui();
    // if (is_keyboard_left()) {
        // render_solidmatrix();
    // } else {
        // render_solidmatrix();
    // }
}
