#if defined(QUANTUM_PAINTER_ENABLE)

// #include "qp.h"
#include "qp_widget_rgbstat.h"
#include "qp_includes.h"
#include "qp_custom_api.h"
#include "defines.h"
#include "rgb_matrix_name.h"

extern painter_device_t my_display;

bool qp_widget_rgbstat_flag = false;

void widget_rgb_init(void) {
    qp_roundrect(WIDGET_RGB_POSX + UI_WIDGET_SHADOW,
                 WIDGET_RGB_POSY + UI_WIDGET_SHADOW,
                 WIDGET_RGB_POSX + WIDGET_RGB_WIDTH  + UI_WIDGET_SHADOW,
                 WIDGET_RGB_POSY + WIDGET_RGB_HEIGHT + UI_WIDGET_SHADOW,
                 UI_COLOR_SHADOW, true, 5, true, true);
    qp_roundrect(WIDGET_RGB_POSX, WIDGET_RGB_POSY, WIDGET_RGB_POSX + WIDGET_RGB_WIDTH, WIDGET_RGB_POSY + WIDGET_RGB_HEIGHT/2, WIDGET_RGB_COLOR1, true, 5, true, false);
    qp_drawtext_recolor(my_display, 12, 131, robotobold25, "RGB", UI_COLOR_TEXT_H1, WIDGET_RGB_COLOR1);
	qp_roundrect(WIDGET_RGB_POSX, WIDGET_RGB_POSY+36, WIDGET_RGB_POSX + WIDGET_RGB_WIDTH, WIDGET_RGB_POSY + WIDGET_RGB_HEIGHT, WIDGET_RGB_COLOR2, true, 5, false, true);
    qp_drawimage_recolor(my_display, 15, WIDGET_RGB_POSY+WIDGET_RGB_HEIGHT/2+2, ico32_brightness,
                              WIDGET_RGB_COLOR2,
							  HSV_BLACK);
}

void widget_rgb_render(void) {
    char buf1[50] = {0};
    if (rgb_matrix_is_enabled()) {
        sprintf(buf1, "#%.2u", rgb_matrix_get_mode());
    } else {
        sprintf(buf1, "OFF");
    }
    qp_drawtext_recolor(my_display, 65, 131, robotobold25, buf1, UI_COLOR_TEXT_H1, WIDGET_RGB_COLOR1);
    
    char buf2[50] = {0};
    sprintf(buf2, "%3u%%", rgb_matrix_get_val()*100/RGB_MATRIX_MAXIMUM_BRIGHTNESS);
	qp_drawtext_recolor(my_display, 50, 167, roboto25, buf2, HSV_BLACK, WIDGET_RGB_COLOR2);
    // char buf2[50] = {0};
    // sprintf(buf2, "Color:  %u-%u-%u", rgb_matrix_get_hue(), rgb_matrix_get_sat(), rgb_matrix_get_val());
    // qp_drawtext(my_display, 0, 220, roboto20, buf2);
}

#endif // defined(QUANTUM_PAINTER_ENABLE)

// sprintf(buf1, "RGB Mode: %s", rgb_matrix_name(rgb_matrix_get_mode()));