#pragma once

#define WIDGET_STATUS_POSX    220
#define WIDGET_STATUS_POSY    5

#define WIDGET_STATUS_FONT    nanoplex16

void widget_status_init(void);
void widget_status_update(uint16_t last_pos, uint16_t new_pos);
void widget_status_render_uptime(void);
