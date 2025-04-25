#ifndef ENCODER_H
#define ENCODER_H

#include "quantum.h"

#define ENCODER_FUNC_MAX 5
#define ENCODER_TAP_FN   SS_TAP(X_MUTE)

/*** Encoder's rotating uses ***/
#define ENCODER_FN1_CW  KC_VOLU
#define ENCODER_FN1_CCW KC_VOLD

#define ENCODER_FN2_CW  KC_PGDN
#define ENCODER_FN2_CCW KC_PGUP

#define ENCODER_FN3_CW  KC_DOWN
#define ENCODER_FN3_CCW KC_UP

#define ENCODER_FN4_CW  KC_BRIU
#define ENCODER_FN4_CCW KC_BRID

#define ENCODER_FN5_CW  KC_RBRC
#define ENCODER_FN5_CCW KC_LBRC

static const char * const encoder_func_name[ENCODER_FUNC_MAX+1] = {
	"    disabled    ",
	"    - VOL +     ",
	"  SCROLL (fast) ",
	"  SCROLL (slow) ",
	" - BRIGHTNESS + ",
	"    [ and ]     "
};

static const char * const encoder_func_shortname[ENCODER_FUNC_MAX+1] = {
	"---",
	"VOL",
	"SCR",
	"SCR",
	"BRT",
	"[ ]"
};

bool process_encoder_rotate(uint8_t index, bool clockwise);
bool process_encoder_tap(void);

#endif /* ENCODER_H */

/****
// Add this to <keyboard.c>
bool encoder_update_kb(uint8_t index, bool clockwise) { // Rotating
	//...
	process_encoder_rotate(index, clockwise);
	//...
}
****/