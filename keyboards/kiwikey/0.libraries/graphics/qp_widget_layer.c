#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp_widget_layer.h"
#include "qp_includes.h"
#include "qp_custom_api.h"

extern painter_device_t my_display;

uint8_t layer_color_hue[4] = { 128, 213, 43, 85 };

void widget_layer_init(void) {
	qp_roundrect(WIDGET_LAYER_POSX + WIDGET_LAYER_SHADOW,
                 WIDGET_LAYER_POSY + WIDGET_LAYER_SHADOW,
                 WIDGET_LAYER_POSX + WIDGET_LAYER_SIZE+WIDGET_LAYER_SHADOW,
                 WIDGET_LAYER_POSY + WIDGET_LAYER_SIZE+WIDGET_LAYER_SHADOW,
                 0,0,100, true, 5, true, true);
	qp_roundrect(WIDGET_LAYER_POSX, WIDGET_LAYER_POSY, WIDGET_LAYER_POSX + WIDGET_LAYER_SIZE, WIDGET_LAYER_POSY + WIDGET_LAYER_SIZE/3, HSV_WHITE, true, 5, true, false);
	qp_drawtext_recolor(my_display, 25, 9, thintel50, "LAYER", WIDGET_LAYER_TEXTCOLOR, HSV_WHITE);
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
	// widget_layer_render(0);
}

void widget_layer_render(uint8_t layer) {
	for (uint8_t i = 0; i < 4; i++) {
		char buf1[10] = {0};
		sprintf(buf1, "%d", i);
		qp_drawtext_recolor(my_display, 12+i*27, 45,    // Write layer number
                 	thintel50, buf1,
                 	0, 0, (layer == i) ? 0 : 200,
                 	layer_color_hue[i], 255, 255);
	}
	qp_roundrect(WIDGET_LAYER_POSX,    // Draw layer_name's background
                 WIDGET_LAYER_POSY+WIDGET_LAYER_SIZE*2/3,
                 WIDGET_LAYER_POSX+WIDGET_LAYER_SIZE,
                 WIDGET_LAYER_POSY+WIDGET_LAYER_SIZE,
                 layer_color_hue[layer], 255, 255, true, 5, false, true);
	// Write layer_name
	qp_drawtext_recolor(my_display, 15, 79, thintel50, layer_name[layer], WIDGET_LAYER_TEXTCOLOR, layer_color_hue[layer], 255, 255);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
