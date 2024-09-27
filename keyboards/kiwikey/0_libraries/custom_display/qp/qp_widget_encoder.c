#if defined(QUANTUM_PAINTER_ENABLE)

// #include "qp.h"
#include "qp_widget_encoder.h"
#include "eeprom_custom.h"
#include "encoder.h"
#include "qp_includes.h"
#include "qp_custom_api.h"
#include "defines.h"
// #include "rgb_matrix_name.h"

extern painter_device_t my_display;

void widget_encoder_init(void) {
    // qp_roundrect(WIDGET_ENCODER_POSX + UI_WIDGET_SHADOW,
                 // WIDGET_ENCODER_POSY + UI_WIDGET_SHADOW,
                 // WIDGET_ENCODER_POSX + WIDGET_ENCODER_WIDTH  + UI_WIDGET_SHADOW,
                 // WIDGET_ENCODER_POSY + WIDGET_ENCODER_HEIGHT + UI_WIDGET_SHADOW,
                 // UI_COLOR_SHADOW, true, 5, true, true);
    // qp_roundrect(WIDGET_ENCODER_POSX, WIDGET_ENCODER_POSY, WIDGET_ENCODER_POSX + WIDGET_ENCODER_WIDTH, WIDGET_ENCODER_POSY + WIDGET_ENCODER_HEIGHT/2, WIDGET_ENCODER_COLOR1, true, 5, true, false);
	// qp_roundrect(WIDGET_ENCODER_POSX, WIDGET_ENCODER_POSY+36, WIDGET_ENCODER_POSX + WIDGET_ENCODER_WIDTH, WIDGET_ENCODER_POSY + WIDGET_ENCODER_HEIGHT, WIDGET_ENCODER_COLOR2, true, 5, false, true);
    qp_circle(my_display, 158, 161, 35, WIDGET_ENCODER_KNOBCOLOROUT, true);
	qp_circle(my_display, 158, 161, 30, WIDGET_ENCODER_KNOBCOLORIN, true);
	qp_drawimage_recolor(my_display, 135+74, 135, ico16_arrow_up, HSV_BLACK, HSV_WHITE);
	qp_drawimage_recolor(my_display, 135+74, 178, ico16_arrow_down, HSV_BLACK, HSV_WHITE);
}

void widget_encoder_render(void) {
    // char buf1[50] = {0};
	// sprintf(buf1, "_%s", encoder_func_shortname);
    // char buf1[50] = {0};
	// sprintf(buf1, "< %s>", encoder_func_shortname[eepdata.knob_func]);
	// qp_drawtext_recolor(my_display, 131, 131,
						// robotobold25, buf1,
						// HSV_BLACK,
						// WIDGET_ENCODER_COLOR1);
	qp_drawtext_recolor(my_display, 135, 150,
						roboto20, "MUTE",
						HSV_BLACK,
						WIDGET_ENCODER_KNOBCOLORIN);
	qp_drawtext_recolor(my_display, 135+62, 150,
						roboto20, encoder_func_shortname[eepdata.knob_func],
						HSV_WHITE,
						HSV_BLACK);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
