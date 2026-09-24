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
	widget_layer_render_layername(get_highest_layer(layer_state), WIDGET_LAYER_POSX, WIDGET_LAYER_POSY);

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

void widget_layer_render_layername(uint8_t layer, uint16_t posx, uint16_t posy) { // BIG LAYER NAME
	// Layer's name background
	qp_roundrect(my_display,
				posx,
				posy,
				posx + WIDGET_MATRIX_WIDTH,
				posy + WIDGET_LAYER_HEIGHT,
				WIDGET_LAYER_BG, true,
				WIDGET_LAYER_CORNER, true, true
				);
	char layer_name_upper[16];
	toUppercase(layer_names[layer], layer_name_upper, sizeof(layer_name_upper));
	qp_drawtext_recolor_center(my_display,
							   posx + WIDGET_LAYER_WIDTH/2,
							   posy + WIDGET_LAYER_HEIGHT/2 +2, // +2 for micro refining
							   WIDGET_LAYER_FONT,
							   layer_name_upper,
							   WIDGET_LAYER_TEXT,
							   WIDGET_LAYER_BG);

	// Fixed icon per layer index, next to the name text but still inside the box
	painter_image_handle_t icon_pool[] = {
		ico24_application, ico24_boss,      ico24_calculator, ico24_component, ico24_earth, ico24_favourites,
		ico24_film,        ico24_globe,     ico24_heart,      ico24_music,     ico24_tune,
	};
	painter_image_handle_t icon = icon_pool[layer % (sizeof(icon_pool)/sizeof(icon_pool[0]))];
	qp_drawimage(my_display,
	             posx + WIDGET_LAYER_WIDTH - WIDGET_LAYER_ICON_PADDING - icon->width,
	             posy + (WIDGET_LAYER_HEIGHT - icon->height)/2,
	             icon);
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
			WIDGET_LAYER_NAV_POSY1 -2,
			319,
			WIDGET_LAYER_NAV_POSY1 + WIDGET_LAYER_NAV_FONT->line_height,
			GLOBAL_BG_COLOR, true
	);
	qp_rect(my_display,
			textnav1_posx,
			WIDGET_LAYER_NAV_POSY2 -2,
			WIDGET_LAYER_NAV_POSX2 - 5,
			WIDGET_LAYER_NAV_POSY2 + WIDGET_LAYER_NAV_FONT->line_height,
			GLOBAL_BG_COLOR, true
	);

	qp_drawtext_recolor(my_display,
						textnav1_posx,
						WIDGET_LAYER_NAV_POSY1 -2, // Better alignment
						WIDGET_LAYER_NAV_FONT,
						layer_names[prev_layer],
						HSV_CYAN,
						GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display,
						textnav2_posx,
						WIDGET_LAYER_NAV_POSY2 -2, // Better alignment
						WIDGET_LAYER_NAV_FONT,
						layer_names[next_layer],
						HSV_CYAN,
						GLOBAL_BG_COLOR);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
