// RGB Matrix naming, copy from @tzarc
#if defined(RGB_MATRIX_ENABLE)
#include "rgb_matrix_name.h"
#include <rgb_matrix.h>

#if defined(RGB_MATRIX_EFFECT)
#    undef RGB_MATRIX_EFFECT
#endif // defined(RGB_MATRIX_EFFECT)
#define RGB_MATRIX_EFFECT(x) RGB_MATRIX_EFFECT_##x,

enum {
    RGB_MATRIX_EFFECT_NONE,
#include "rgb_matrix_effects.inc"
#undef RGB_MATRIX_EFFECT
#ifdef RGB_MATRIX_CUSTOM_KB
#    include "rgb_matrix_kb.inc"
#endif
#ifdef RGB_MATRIX_CUSTOM_USER
#    include "rgb_matrix_user.inc"
#endif
};

#define RGB_MATRIX_EFFECT(x)    \
    case RGB_MATRIX_EFFECT_##x: \
        return #x;
const char* rgb_matrix_name(uint8_t effect) {
    switch (effect) {
        case RGB_MATRIX_EFFECT_NONE:
            return "NONE";
#include "rgb_matrix_effects.inc"
#undef RGB_MATRIX_EFFECT
#ifdef RGB_MATRIX_CUSTOM_KB
#    include "rgb_matrix_kb.inc"
#endif
#ifdef RGB_MATRIX_CUSTOM_USER
#    include "rgb_matrix_user.inc"
#endif
        default:
            return "UNKNOWN";
    }
}
#endif // defined(RGB_MATRIX_ENABLE)

// const char rgbmode_names[16][21] = {
    // "All Off",
    // "Solid Color",
    // "Breathing",
    // "Cycle All",
    // "Cycle Left/Right",
    // "Cycle Up/Down",
    // "Dual Beacon",
    // "Rainbow Beacon",
    // "Raindrops",
    // "Typing Heatmap",
    // "Solid Reactive Simple",
    // "Solid Reactive",
    // "Solid Reactive Cross",
    // "Solid Reactive Nexus",
    // "Splash",
    // "Solid Splash"
// };
