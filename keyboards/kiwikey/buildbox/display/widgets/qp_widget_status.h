#pragma once

#include "qp_widget_layer.h"

#define WIDGET_STATUS_POSX          220
#define WIDGET_STATUS_VALUE_POSX    WIDGET_STATUS_POSX + 65
#define WIDGET_STATUS_POSY          WIDGET_LAYER_POSY + 3

#define WIDGET_STATUS_FONT          nanoplex16
#define WIDGET_STATUS_LABEL_COLOR   HSV_WHITE
#define WIDGET_STATUS_VALUE_COLOR   HSV_CYAN

#define WIDGET_STATUS_LABEL1  "RGB MODE   "
#define WIDGET_STATUS_LABEL2  "BRIGHTNESS "
#define WIDGET_STATUS_LABEL3  "SLEEP      "

void widget_status_init(void);
void widget_status_update(void);
