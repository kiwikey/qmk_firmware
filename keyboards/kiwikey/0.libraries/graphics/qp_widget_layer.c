#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp_widget_layer.h"
#include "qp_includes.h"
#include "qp_custom_api.h"
#include "defines.h"

extern painter_device_t my_display;

bool qp_widget_layer_flag = false;
uint8_t layer_color_hue[DYNAMIC_KEYMAP_LAYER_COUNT] = { 128, 213, 43, 85 }; // TODO

void widget_layer_init(void) {
	qp_roundrect(WIDGET_LAYER_POSX + UI_WIDGET_SHADOW,
                 WIDGET_LAYER_POSY + UI_WIDGET_SHADOW,
                 WIDGET_LAYER_POSX + WIDGET_LAYER_SIZE+UI_WIDGET_SHADOW,
                 WIDGET_LAYER_POSY + WIDGET_LAYER_SIZE+UI_WIDGET_SHADOW,
                 UI_COLOR_SHADOW, true, 5, true, true);
	qp_roundrect(WIDGET_LAYER_POSX, WIDGET_LAYER_POSY, WIDGET_LAYER_POSX + WIDGET_LAYER_SIZE, WIDGET_LAYER_POSY + WIDGET_LAYER_SIZE/3, WIDGET_LAYER_COLOR1, true, 5, true, false);
	// qp_drawtext_recolor(my_display, 25, 9, thintel50, "LAYER", WIDGET_LAYER_TEXTCOLOR, HSV_WHITE);
	qp_drawtext_recolor(my_display, 22, 11, robotobold25, "LAYER", UI_COLOR_TEXT_H1, WIDGET_LAYER_COLOR1);
	for (uint8_t i = 0; i < 4; i++) {
		qp_rect(my_display,
                 WIDGET_LAYER_POSX + i*WIDGET_LAYER_SIZE/4,
                 WIDGET_LAYER_POSY + (WIDGET_LAYER_SIZE/3)+1,
                 WIDGET_LAYER_POSX + WIDGET_LAYER_SIZE/4*(i+1),
                 WIDGET_LAYER_POSY + WIDGET_LAYER_SIZE*2/3,
                 layer_color_hue[i], 255, 255, true);
	}
	qp_roundrect(WIDGET_LAYER_POSX,
                 WIDGET_LAYER_POSY + WIDGET_LAYER_SIZE*2/3,
                 WIDGET_LAYER_POSX + WIDGET_LAYER_SIZE,
                 WIDGET_LAYER_POSY + WIDGET_LAYER_SIZE,
                 layer_color_hue[0], 255, 255, true, 5, false, true);
}

void widget_layer_render(uint8_t layer) {
	for (uint8_t i = 0; i < 4; i++) {
		char buf1[10] = {0};
		sprintf(buf1, "%d", i);
		// qp_drawtext_recolor(my_display, 12+i*27, 45,    // Write layer number
                 	// thintel50, buf1,
                 	// 0, 0, (layer == i) ? 0 : 200,
                 	// layer_color_hue[i], 255, 255);
		qp_drawtext_recolor(my_display, 12+i*27, 47,    // Write layer number
							robotobold25, buf1,
							0, 0, (layer == i) ? 0 : 200,
							layer_color_hue[i], 255, 255);
	}
	qp_roundrect(WIDGET_LAYER_POSX,    // Draw background of layer_name
                 WIDGET_LAYER_POSY+WIDGET_LAYER_SIZE*2/3,
                 WIDGET_LAYER_POSX+WIDGET_LAYER_SIZE,
                 WIDGET_LAYER_POSY+WIDGET_LAYER_SIZE,
                 layer_color_hue[layer], 255, 255, true, 5, false, true);
	// Write layer_name
	// qp_drawtext_recolor(my_display, 15, 79, thintel50, layer_name[layer], WIDGET_LAYER_TEXTCOLOR, layer_color_hue[layer], 255, 255);
	qp_drawtext_recolor(my_display, 18, 85, roboto20, layer_name[layer], UI_COLOR_TEXT_H2, layer_color_hue[layer], 255, 255);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
