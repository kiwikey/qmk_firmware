#ifndef QP_WIDGET_RGBSTAT
#define QP_WIDGET_RGBSTAT

#include "quantum.h"

#define WIDGET_RGB_POSX      5
#define WIDGET_RGB_POSY      125
#define WIDGET_RGB_WIDTH     108
#define WIDGET_RGB_HEIGHT    72

extern bool qp_widget_rgbstat_flag;

void widget_rgb_init(void);
void widget_rgb_render(void);

#endif /* QP_WIDGET_RGBSTAT */
