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
	widget_layer_render_layername(get_highest_layer(layer_state));

	qp_drawimage_recolor(my_display,
						WIDGET_LAYER_NAV_POSX1,
						WIDGET_LAYER_NAV_POSY1,
						ico12_arrow_left, GLOBAL_BG_COLOR, HSV_WHITE);
	qp_drawimage_recolor(my_display,
						WIDGET_LAYER_NAV_POSX2,
						WIDGET_LAYER_NAV_POSY2,
						ico12_arrow_right, GLOBAL_BG_COLOR, HSV_WHITE);
	widget_layer_render_navigation(get_highest_layer(layer_state));
}

void widget_layer_render_layername(uint8_t layer) { // BIG LAYER NAME
	// Layer's name background
	qp_roundrect(my_display,
				WIDGET_LAYER_POSX,
				WIDGET_LAYER_POSY,
				WIDGET_LAYER_POSX + WIDGET_MATRIX_WIDTH,
				WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT,
				WIDGET_LAYER_BG, true,
				WIDGET_LAYER_CORNER, true, true
				);
	qp_drawtext_recolor_center(my_display,
							   WIDGET_LAYER_POSX + WIDGET_LAYER_WIDTH/2,
							   WIDGET_LAYER_POSY + WIDGET_LAYER_HEIGHT/2 +2, // +2 for micro refining
							   WIDGET_LAYER_FONT,
							   toUppercase(layer_names[layer]),
							   WIDGET_LAYER_TEXT,
							   WIDGET_LAYER_BG);
}

void widget_layer_render_navigation(uint8_t layer) {
	uint8_t prev_layer;
	uint8_t next_layer;

	if (layer <= 0) // layer 0
		prev_layer = DYNAMIC_KEYMAP_LAYER_COUNT-1;
	else prev_layer = layer - 1;

	if (layer >= DYNAMIC_KEYMAP_LAYER_COUNT-1) // last layer
		next_layer = 0;
	else next_layer = layer + 1;
	
	uint16_t textnav1_posx = WIDGET_LAYER_NAV_POSX1 + ico12_arrow_left->width + 5;
	uint16_t textnav2_posx = WIDGET_LAYER_NAV_POSX2 - qp_textwidth(WIDGET_LAYER_NAV_FONT, layer_names[next_layer]) - 5;

	qp_rect(my_display,
			textnav1_posx,
			WIDGET_LAYER_NAV_POSY1,
			319,
			WIDGET_LAYER_NAV_POSY1 + WIDGET_LAYER_NAV_FONT->line_height,
			GLOBAL_BG_COLOR, true
	);
	qp_rect(my_display,
			textnav1_posx,
			WIDGET_LAYER_NAV_POSY2,
			WIDGET_LAYER_NAV_POSX2 - 5,
			WIDGET_LAYER_NAV_POSY2 + WIDGET_LAYER_NAV_FONT->line_height,
			GLOBAL_BG_COLOR, true
	);

	qp_drawtext_recolor(my_display,
						textnav1_posx,
						WIDGET_LAYER_NAV_POSY1 +1, // Better alignment
						WIDGET_LAYER_NAV_FONT,
						layer_names[prev_layer],
						HSV_WHITE,
						GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display,
						textnav2_posx,
						WIDGET_LAYER_NAV_POSY2 +1, // Better alignment
						WIDGET_LAYER_NAV_FONT,
						layer_names[next_layer],
						HSV_WHITE,
						GLOBAL_BG_COLOR);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
