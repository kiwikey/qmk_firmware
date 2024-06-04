// Copyright 2023 KiwiKey
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef OLED_CUSTOM_API
#define OLED_CUSTOM_API

#include "quantum.h"

#define ALIGN_LEFT   0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT  2

void oled_pan_area(char start_x, char end_x, char start_y, char end_y, bool left);
void draw_line_h(uint8_t x, uint8_t y, uint8_t len, bool on);
void draw_line_v(uint8_t x, uint8_t y, uint8_t len, bool on);
void draw_rect(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey, bool on);
void oled_write_align(const char *data, uint8_t align, bool invert);
void oled_write_align_P(const char *data, uint8_t align, bool invert);

#endif /* OLED_CUSTOM_API */
