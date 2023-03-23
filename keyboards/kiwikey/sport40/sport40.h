#pragma once
#include "quantum.h"
#include "qp.h"

extern painter_device_t my_display;

#define LAYOUT( \
	k00, k01, k02, k03, k04, k05, \
	k10, k11, k12, k13, k14, k15, \
	k20, k21, k22, k23, k24, k25, \
	k30, k31, k32, k33, k34, k35  \
) { \
	{ k00, k01, k02, k03, k04, k05 }, \
	{ k10, k11, k12, k13, k14, k15 }, \
	{ k20, k21, k22, k23, k24, k25 }, \
	{ k30, k31, k32, k33, k34, k35 }  \
}
