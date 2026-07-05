#if defined(QUANTUM_PAINTER_ENABLE)

#include "quantum.h"
#include "qp_widget_layer.h"

#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"

// bool qp_widget_layer_flag = false;
uint8_t layer_color_hue[DYNAMIC_KEYMAP_LAYER_COUNT] = { 128, 213, 43, 85 }; // TODO

void widget_layer_init(void) {
	for (uint8_t i = 0; i < 4; i++) {
		qp_rect(my_display,
                 WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4,
                 WIDGET_LAYER_POSY,
                 WIDGET_LAYER_POSX + (i+1)*WIDGET_LAYER_WIDTH/4,
                 WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
                 WIDGET_LAYER_OUTLINE, false);
	}
	widget_layer_render_layername(get_highest_layer(layer_state));
}

void widget_layer_render(uint8_t layer) {
	// Write layer number
	for (uint8_t i = 0; i < 4; i++) {
		char buf1[2] = {0};
		sprintf(buf1, "%d", i);
		if (layer == i) { // This IF should be shorten
			qp_rect(my_display,
					 WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4 + 1,
					 WIDGET_LAYER_POSY + 1,
					 WIDGET_LAYER_POSX + (i+1)*WIDGET_LAYER_WIDTH/4, // no need -1
					 WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT - 1,
					 layer_color_hue[i], 255, 255,
					 true);
					//  WIDGET_LAYER_OUTLINE, true);
			qp_drawtext_recolor(my_display,
								WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4 + 3,
								WIDGET_LAYER_POSY + 3,
								robotobold25, buf1,
								WIDGET_LAYER_ON_TEXT,
								layer_color_hue[i], 255, 255);
		} else {
			qp_rect(my_display,
					 WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4,
					 WIDGET_LAYER_POSY,
					 WIDGET_LAYER_POSX + (i+1)*WIDGET_LAYER_WIDTH/4,
					 WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
					 WIDGET_LAYER_BG, true);
			qp_rect(my_display,
					 WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4,
					 WIDGET_LAYER_POSY,
					 WIDGET_LAYER_POSX + (i+1)*WIDGET_LAYER_WIDTH/4,
					 WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
					 WIDGET_LAYER_OUTLINE, false);
			qp_drawtext_recolor(my_display,
								WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4 + 3,
								WIDGET_LAYER_POSY + 3,
								robotobold25, buf1,
								WIDGET_LAYER_OFF_TEXT,
								HSV_BLACK);
		}
	}
	widget_layer_render_layername(layer);
}

void widget_layer_render_layername(uint8_t layer) {
	// Background first
	qp_rect(my_display,
				WIDGET_LAYER_POSX + WIDGET_LAYER_WIDTH,
				WIDGET_LAYER_POSY,
				WIDGET_LAYER_POSX + WIDGET_MATRIX_WIDTH,
				WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
				layer_color_hue[layer], 255, 255,
				true);
	// Then outline
	qp_rect(my_display,
				WIDGET_LAYER_POSX + WIDGET_LAYER_WIDTH,
				WIDGET_LAYER_POSY,
				WIDGET_LAYER_POSX + WIDGET_MATRIX_WIDTH,
				WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
				WIDGET_LAYER_OUTLINE, false);
	qp_drawtext_recolor_center(my_display,
							   WIDGET_LAYER_POSX + (WIDGET_MATRIX_WIDTH + WIDGET_LAYER_WIDTH)/2,
							   WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT/2,
							   roboto20,
							   layer_name[layer],
							   HSV_BLACK,
							   layer_color_hue[layer], 255, 255);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
