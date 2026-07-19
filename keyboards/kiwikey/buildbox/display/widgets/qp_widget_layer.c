#if defined(QUANTUM_PAINTER_ENABLE)

#include "quantum.h"
#include "qp_widget_layer.h"

#include "features/eeprom_custom.h"
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"

// bool qp_widget_layer_flag = false;

void widget_layer_init(void) {
	// Layer's number outline
	for (uint8_t i = 0; i < 4; i++) {
		qp_rect(my_display,
                 WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4,
                 WIDGET_LAYER_POSY,
                 WIDGET_LAYER_POSX + (i+1)*WIDGET_LAYER_WIDTH/4,
                 WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
                 WIDGET_LAYER_OUTLINE, false);
	}
	// Layer's name outline
	qp_rect(my_display,
				WIDGET_LAYER_POSX + WIDGET_LAYER_WIDTH,
				WIDGET_LAYER_POSY,
				WIDGET_LAYER_POSX + WIDGET_MATRIX_WIDTH,
				WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
				WIDGET_LAYER_OUTLINE, false);
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
					 eepdata.layer_hue[i], eepdata.layer_sat[i], 255,
					 true);
			qp_drawtext_recolor(my_display,
								WIDGET_LAYER_POSX + i*WIDGET_LAYER_WIDTH/4 + 3,
								WIDGET_LAYER_POSY + 3,
								robotobold25, buf1,
								WIDGET_LAYER_ON_TEXT,
								eepdata.layer_hue[i], eepdata.layer_sat[i], 255);
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
	// Write layer name
	widget_layer_render_layername(layer);
}

// This function is for shortening purpose only,
// used only for LCD rendering when layer' color changed
void widget_layer_number_render(uint8_t layer) {
	// Write layer number
	char buf1[4] = {0};
	sprintf(buf1, "%d", layer);
	qp_rect(my_display,
				WIDGET_LAYER_POSX + layer*WIDGET_LAYER_WIDTH/4 + 1,
				WIDGET_LAYER_POSY + 1,
				WIDGET_LAYER_POSX + (layer+1)*WIDGET_LAYER_WIDTH/4, // no need -1
				WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT - 1,
				eepdata.layer_hue[layer], eepdata.layer_sat[layer], 255,
				true);
	qp_drawtext_recolor(my_display,
						WIDGET_LAYER_POSX + layer*WIDGET_LAYER_WIDTH/4 + 3,
						WIDGET_LAYER_POSY + 3,
						robotobold25, buf1,
						WIDGET_LAYER_ON_TEXT,
						eepdata.layer_hue[layer], eepdata.layer_sat[layer], 255);
}

void widget_layer_render_layername(uint8_t layer) {
	// Background (+1 and -1 is for not touching the outline)
	qp_rect(my_display,
				WIDGET_LAYER_POSX + WIDGET_LAYER_WIDTH + 1,
				WIDGET_LAYER_POSY + 1,
				WIDGET_LAYER_POSX + WIDGET_MATRIX_WIDTH - 1,
				WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT - 1,
				eepdata.layer_hue[layer], eepdata.layer_sat[layer], 255,
				true);
	// Then text
	qp_drawtext_recolor_center(my_display,
							   WIDGET_LAYER_POSX + (WIDGET_MATRIX_WIDTH + WIDGET_LAYER_WIDTH)/2,
							   WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT/2,
							   roboto20,
							   layer_names[layer],
							   HSV_BLACK,
							   eepdata.layer_hue[layer], eepdata.layer_sat[layer], 255);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
