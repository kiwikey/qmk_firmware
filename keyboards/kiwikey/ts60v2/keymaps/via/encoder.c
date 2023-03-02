#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][1][2] = {
    [0] = { ENCODER_CCW_CW( KC_VOLU,  KC_VOLD) },
    [1] = { ENCODER_CCW_CW( KC_PGDN,  KC_PGUP) },
    [2] = { ENCODER_CCW_CW( RGB_VAD,  RGB_VAI) },
    [3] = { ENCODER_CCW_CW( KC_TRNS,  KC_TRNS) },
};
#endif
