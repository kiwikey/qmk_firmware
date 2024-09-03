#include QMK_KEYBOARD_H

#if defined(QUANTUM_PAINTER_ENABLE)

#include <printf.h>
#include "qp.h"
#include "qp_graphics.h"
#include "qp_custom_api.h"
#include "qp_includes.h"

#include "qp_widget_layer.h"
#include "qp_widget_matrix.h"
#include "qp_menu.h"
#include "rgb_matrix_name.h"

void keyboard_post_init_display(void) {
	qp_init_load_files();
	
	// Background
	qp_drawimage(my_display, 0, 0, img_pikachu);
	
	widget_layer_init();
	widget_layer_render(0); // TODO: render the layer get from EEPROM
	widget_matrix_init();

	// qp_rect(my_display, UI_LAYER_BG_X, UI_LAYER_BG_Y, UI_LAYER_BG_X+108, UI_LAYER_BG_Y+36, UI_LAYER_COLOR0, true);
	// qp_drawtext_recolor(my_display, 13, 44+36, arial30, "0", UI_LAYER_TEXTCOLOR, HSV_CYAN);
}

void housekeeping_task_display(void) {
	if (last_matrix_activity_elapsed() > QUANTUM_PAINTER_DISPLAY_TIMEOUT) {
		backlight_level(0);
    }
	
	ui_render_rgbmatrix_info();
	
	// uint16_t curr_hue = 100;
	// static led_t last_led_state = {0};
	// last_led_state.raw = host_keyboard_led_state().raw;
	// qp_drawimage_recolor(my_display, 12, 120, last_led_state.caps_lock ? lock_caps_on : lock_caps_off, curr_hue, 255, last_led_state.caps_lock ? 255 : 32, curr_hue, 255, 0);
	// qp_drawimage_recolor(my_display, 12+32, 120, last_led_state.num_lock ? lock_num_on : lock_num_off, curr_hue, 255, last_led_state.num_lock ? 255 : 32, curr_hue, 255, 0);
	// qp_drawimage_recolor(my_display, 12+32+32, 120, last_led_state.scroll_lock ? lock_scrl_on : lock_scrl_off, curr_hue, 255, last_led_state.scroll_lock ? 255 : 32, curr_hue, 255, 0);
	
	
	// char buf1[50] = {0};
    // sprintf(buf1, "- Key_Timer: %lu   ", last_matrix_activity_elapsed());
	// qp_drawtext(my_display, 12, 180, thintel, buf1);
	
	// buf1[50] = {0};
	// sprintf(buf1, "- RGB Matrix effect: %s   ", rgb_matrix_name(rgb_matrix_get_mode());
	// qp_drawtext(my_display, 0, 200, thintel, "                                                             ");
	// qp_drawtext(my_display, 0, 200, thintel, rgb_matrix_name(rgb_matrix_get_mode()) );

}

bool process_record_display(uint16_t keycode, keyrecord_t *record) {
	if (record->event.pressed) {
		backlight_level(10);
	}
	widget_matrix_update(record->event.key.col, record->event.key.row);

	return true;
}

layer_state_t layer_state_set_display(layer_state_t state) {
	widget_layer_render(get_highest_layer(state));
	return state;
}

void ui_render_rgbmatrix_info(void) {
	char buf1[50] = {0};
	sprintf(buf1, "RGB Mode: %s", rgb_matrix_name(rgb_matrix_get_mode()));
	qp_drawtext(my_display, 0, 200, thintel, buf1);
	
	char buf2[50] = {0};
	sprintf(buf2, "RGB HSV:  %u - %u - %u", rgb_matrix_get_hue(), rgb_matrix_get_sat(), rgb_matrix_get_val());
	qp_drawtext(my_display, 0, 220, thintel, buf2);	
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
