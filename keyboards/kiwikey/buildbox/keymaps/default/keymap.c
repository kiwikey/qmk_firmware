#include QMK_KEYBOARD_H

// Layer order/count must match layer_names[] (display/widgets/qp_widget_layer.c)
// and DYNAMIC_KEYMAP_LAYER_COUNT (config.h). The last row is the 2 buttons
// below the screen - keep them on every layer: they switch layers and open
// the Settings Menu (Button 1 + Button 2).
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0] = LAYOUT_numpad_4x4 ( // Default
		KC_P7,   KC_P8,   KC_P9,   KC_KP_PLUS,
		KC_P4,   KC_P5,   KC_P6,   KC_KP_MINUS,
		KC_P1,   KC_P2,   KC_P3,   KC_KP_ASTERISK,
		KC_P0,   KC_PDOT, KC_PENT, KC_KP_SLASH,
		KC_BUTTON_1, KC_BUTTON_2
	),
	[1] = LAYOUT_numpad_4x4 ( // Media
		KC_MPRV, KC_MPLY, KC_MNXT, KC_MSTP,
		KC_VOLD, KC_MUTE, KC_VOLU, KC_MSEL,
		KC_WBAK, KC_WHOM, KC_WFWD, KC_WREF,
		KC_CALC, KC_MYCM, KC_NO,   KC_NO,
		KC_BUTTON_1, KC_BUTTON_2
	),
	[2] = LAYOUT_numpad_4x4 ( // Mouse
		MS_BTN1, MS_UP,   MS_BTN2, MS_WHLU,
		MS_LEFT, MS_DOWN, MS_RGHT, MS_WHLD,
		MS_WHLL, KC_NO,   MS_WHLR, KC_NO,
		KC_NO,   KC_NO,   KC_NO,   KC_NO,
		KC_BUTTON_1, KC_BUTTON_2
	),
	[3] = LAYOUT_numpad_4x4 ( // RGB LED
		RM_TOGG, RM_PREV, RM_NEXT, KC_NO,
		RM_HUED, RM_HUEU, KC_NO,   KC_NO,
		RM_VALD, RM_VALU, KC_NO,   KC_NO,
		KC_NO,   KC_NO,   KC_NO,   KC_NO,
		KC_BUTTON_1, KC_BUTTON_2
	)
};
