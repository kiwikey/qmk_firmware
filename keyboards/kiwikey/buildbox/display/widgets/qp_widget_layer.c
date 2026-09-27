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
							   posy + WIDGET_LAYER_HEIGHT/2 +2,
							   WIDGET_LAYER_FONT,
							   layer_name_upper,
							   WIDGET_LAYER_TEXT,
							   WIDGET_LAYER_BG);

	// Chosen icon for this layer, next to the name text but still inside the box -
	// picked in the "LAYERS CONFIG" menu's icon picker (qp_menu.c) and persisted
	// in eepdata.layer_icon[]. layer_icon_pool_icon()/layer_icon_get_choice() are
	// the single source of truth, shared with that picker.
	painter_image_handle_t icon = layer_icon_pool_icon(layer_icon_get_choice(layer));
	qp_drawimage(my_display,
	             posx + WIDGET_LAYER_ICON_PADDING,
	             posy + (WIDGET_LAYER_HEIGHT - icon->height)/2 +1,
	             icon);
}

// 22x22 batch (display/resources/icons/22x22/, see qp_includes.h/.c) - the
// picker in qp_menu.c scrolls through this same pool, index for index.
painter_image_handle_t layer_icon_pool_icon(uint8_t pool_index) {
	painter_image_handle_t pool[LAYER_ICON_POOL_COUNT] = {
		ico22_25,          ico22_47,              ico22_68,          ico22_72,    ico22_74,
		ico22_86,          ico22_88,              ico22_application, ico22_boss,  ico22_calculator,
		ico22_color_wheel, ico22_component,       ico22_desktop,     ico22_earth, ico22_favourites,
		ico22_film,        ico22_game_controller, ico22_gear2,       ico22_globe, ico22_heart,
		ico22_heart1,      ico22_mail,            ico22_mouse,       ico22_online,
	};
	return pool[pool_index % LAYER_ICON_POOL_COUNT];
}

// Per-layer chosen pool index - backed by eepdata.layer_icon[] (features/
// eeprom_custom.h) so a choice survives a reboot the same way every other
// setting does: it's just written back to EEPROM whenever eeprom_update_custom()
// runs (menu_exit(), qp_menu.c), no extra save logic needed here.
uint8_t layer_icon_get_choice(uint8_t layer) {
	return eepdata.layer_icon[layer % DYNAMIC_KEYMAP_LAYER_COUNT];
}

void layer_icon_set_choice(uint8_t layer, uint8_t pool_index) {
	eepdata.layer_icon[layer % DYNAMIC_KEYMAP_LAYER_COUNT] = pool_index % LAYER_ICON_POOL_COUNT;
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
