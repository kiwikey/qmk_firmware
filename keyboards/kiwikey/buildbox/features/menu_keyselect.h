#pragma once

// TEMPORARY / DRAFT - keyboard-driven Main Menu navigation.
// See menu_keyselect.c for what this does and what's still needed to wire it in.

#include "quantum.h"

// On-page item slots, one grid key each: slot N -> LED index N -> row N/4, col N%4.
// Must be <= MENU_LINESPERPAGE (qp_menu.h) so every slot maps to a real on-page item.
#define MENU_KEYSELECT_ITEM_SLOTS   7

// Dedicated paging keys (LED index on the 4x4 grid = row*4 + col).
#define MENU_KEYSELECT_LED_PAGEPREV 11 // row 2, col 3
#define MENU_KEYSELECT_LED_PAGENEXT 15 // row 3, col 3

void menu_keyselect_highlight(uint8_t led_min, uint8_t led_max);
bool menu_keyselect_process_record(keyrecord_t *record);
