#pragma once

#include "qp_widget_layer.h"

#define WIDGET_STATUS_POSX    230
#define WIDGET_STATUS_POSY    WIDGET_LAYER_POSY + 3

#define WIDGET_STATUS_FONT    nanoplex16

void widget_status_init(void);
void widget_status_update(void);
void widget_status_render_uptime(void);