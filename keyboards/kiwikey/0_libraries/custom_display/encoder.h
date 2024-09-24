#ifndef ENCODER_H
#define ENCODER_H

#include "quantum.h"

#define ENCODER_FUNC_MAX 4
// #define ENCODER_TAP_FN   SS_TAP(X_MUTE)
#define ENCODER_TAP_FN   SS_TAP(X_Z)

/*** Encoder's rotating uses ***/
// #define ENCODER_FN1_CW  KC_VOLU
// #define ENCODER_FN1_CCW KC_VOLD
#define ENCODER_FN1_CW  KC_RIGHT
#define ENCODER_FN1_CCW KC_LEFT

#define ENCODER_FN2_CW  KC_PGDN
#define ENCODER_FN2_CCW KC_PGUP

#define ENCODER_FN3_CW  KC_DOWN
#define ENCODER_FN3_CCW KC_UP

#define ENCODER_FN4_CW  KC_BRIU
#define ENCODER_FN4_CCW KC_BRID

static const char * const encoder_func_name[ENCODER_FUNC_MAX+1] = {
	"    disabled    ",
	"    - VOL +     ",
	"  SCROLL (fast) ",
	"  SCROLL (slow) ",
	" - BRIGHTNESS + "
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