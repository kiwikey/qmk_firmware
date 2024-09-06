#pragma once

#ifndef QP_GRAPHICS
#define QP_GRAPHICS

#define FW_VERSION "2024.09.06_01"

typedef struct {
    uint8_t active_layer;
    uint8_t oled_anim;
    uint8_t oled_timeout;
	uint8_t oled_brightness;
    uint8_t lighting_layers; // 0 = OFF; 1 = ON
	uint8_t lighting_flags;  // 0 = UNDERGLOW; 1 = BACKLIGHT; 2 = ALL LEDS
    uint8_t layer_hue[DYNAMIC_KEYMAP_LAYER_COUNT];
	uint8_t layer_sat[DYNAMIC_KEYMAP_LAYER_COUNT];
} EEPROM_CUSTOM_DATA;

extern EEPROM_CUSTOM_DATA eepdata;

enum keyboard_keycodes {
    CUSTOM_KC_MENU = QK_KB_0,
	CUSTOM_KC_REFRESH,
    // CUSTOM_2,
    // CUSTOM_3,
};

void ui_refresh(void);
void ui_render_rgbmatrix_info(void);

#endif /* QP_GRAPHICS */
