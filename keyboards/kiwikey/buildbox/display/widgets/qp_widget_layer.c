#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp_widget_layer.h"
#include "../qp_includes.h"
#include "../qp_custom_api.h"
#include "../defines.h"

extern painter_device_t my_display;

bool qp_widget_layer_flag = false;
uint8_t layer_color_hue[DYNAMIC_KEYMAP_LAYER_COUNT] = { 128, 213, 43, 85 }; // TODO

void widget_layer_init(void) {
	// qp_roundrect(WIDGET_LAYER_POSX + UI_WIDGET_SHADOW,
                 // WIDGET_LAYER_POSY + UI_WIDGET_SHADOW,
                 // WIDGET_LAYER_POSX + WIDGET_LAYER_WIDTH+UI_WIDGET_SHADOW,
                 // WIDGET_LAYER_POSY + WIDGET_LAYER_WIDTH+UI_WIDGET_SHADOW,
                 // UI_COLOR_SHADOW, true, 5, true, true);
	// qp_roundrect(WIDGET_LAYER_POSX,
				 // WIDGET_LAYER_POSY,
				 // WIDGET_LAYER_POSX + WIDGET_LAYER_WIDTH,
				 // WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
				 // WIDGET_LAYER_COLOR1, true, 5, true, false);
	for (uint8_t i = 0; i < 4; i++) {
		qp_rect(my_display,
                 WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4,
                 WIDGET_LAYER_POSY,
                 WIDGET_LAYER_POSX + (i+1)*WIDGET_LAYER_WIDTH/4,
                 WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
                 HSV_WHITE, false);
	}
}

void widget_layer_render(uint8_t layer) {
	// Write layer number
	for (uint8_t i = 0; i < 4; i++) {
		char buf1[2] = {0};
		sprintf(buf1, "%d", i);
		if (layer == i) { // This IF should be shorten
			qp_rect(my_display,
					 WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4,
					 WIDGET_LAYER_POSY,
					 WIDGET_LAYER_POSX + (i+1)*WIDGET_LAYER_WIDTH/4,
					 WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
					 HSV_WHITE, true);
			qp_drawtext_recolor(my_display,
								WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4 + 3,
								WIDGET_LAYER_POSY + 3,
								robotobold25, buf1,
								HSV_BLACK,
								HSV_WHITE);
		} else {
			qp_rect(my_display,
					 WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4,
					 WIDGET_LAYER_POSY,
					 WIDGET_LAYER_POSX + (i+1)*WIDGET_LAYER_WIDTH/4,
					 WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
					 HSV_BLACK, true);
			qp_drawtext_recolor(my_display,
								WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4 + 3,
								WIDGET_LAYER_POSY + 3,
								robotobold25, buf1,
								HSV_WHITE,
								HSV_BLACK);
		}
	}
	widget_layer_init(); // TODO: clean up this
	// Write layer_name
	qp_drawtext_recolor(my_display, WIDGET_LAYER_POSX + 85, WIDGET_LAYER_POSY + 5, roboto20, layer_name[layer], UI_COLOR_TEXT_H2, layer_color_hue[layer], 255, 255);
}

layer_state_t layer_state_set_kb(layer_state_t state) {
		widget_layer_render(get_highest_layer(state));
	return state;
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
