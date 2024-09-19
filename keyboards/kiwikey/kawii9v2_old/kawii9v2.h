#pragma once

#include "quantum.h"

#define LAYOUT_ortho_3x3( \
    k00, k01, k02, \
    k10, k11, k12, \
    k20, k21, k22, \
	k33, k44  \
) \
{ \
    { k00,   k01,   k02,   KC_NO,   KC_NO }, \
    { k10,   k11,   k12,   KC_NO,   KC_NO }, \
    { k20,   k21,   k22,   KC_NO,   KC_NO }, \
	{ KC_NO, KC_NO, KC_NO, k33,     KC_NO }, \
	{ KC_NO, KC_NO, KC_NO, KC_NO,   k44   }  \
}
