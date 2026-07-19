#include <qp.h>

#include "knob_custom.h"

#include "features/eeprom_custom.h"
#include "sensor/sensors_handler.h"
#include "display/defines.h"
#include "display/qp_custom_api.h"

// Fixed angle (0-255, same space as widget_knob_draw_dot()'s `angle`) that ring
// LED `idx` (0..7) sits at: idx0 = 9 o'clock, going clockwise from there.
static uint8_t knob_led_center_angle(uint8_t idx) {
    return (uint8_t)(128 - 32 * (int16_t)idx);
}

// Shortest signed distance from b to a on a 0-255 circular scale, range (-128, 128].
// Used both for angle math and for hue math since both are mod-256 circular scales.
static int16_t angle_diff(uint8_t a, uint8_t b) {
    int16_t d = (int16_t)a - (int16_t)b;
    d = ((d % 256) + 256) % 256;
    if (d > 128) d -= 256;
    return d;
}

void knob_effect(void) {
    bool show_gradient = magnetic_encoder.is_present && timer_elapsed32(last_knob_movement_time) < KNOB_LED_FLASH_MS;
    hsv_t base = { eepdata.layer_hue[get_highest_layer(layer_state|default_layer_state)], // hue
                   eepdata.layer_sat[get_highest_layer(layer_state|default_layer_state)], // sat
                   255 // val = 100%
                 };

    uint8_t angle    = 255 - (magnetic_encoder.new_angle >> 4);
    int16_t hue_diff = angle_diff(0, base.h); // shortest path from layer hue to red

    for (uint8_t idx = 0; idx < KNOB_LED_COUNT; idx++) {
        uint8_t i = KNOB_LED_START + idx;
        if (!show_gradient) {
            rgb_matrix_set_color_hsv(i, base);
            continue;
        }

        int16_t raw_dist = angle_diff(angle, knob_led_center_angle(idx));
        uint8_t dist     = (uint8_t)(raw_dist >= 0 ? raw_dist : -raw_dist); // abs()
        uint8_t blend = (dist >= KNOB_LED_FALLOFF_ANGLE)
                             ? 0
                             : (uint8_t)(255 - ((uint16_t)dist * 255) / KNOB_LED_FALLOFF_ANGLE);

        uint8_t hue = (uint8_t)(base.h + ((int16_t)hue_diff * blend) / 255);
        uint8_t sat = (uint8_t)(base.s + (((int16_t)255 - base.s) * blend) / 255);
        rgb_matrix_set_color_hsv(i, (hsv_t){hue, sat, 255});
    }
}
