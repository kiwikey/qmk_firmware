#pragma once

#include "quantum.h"

#define TUTORIAL_ENABLE 1

#define TUTORIAL_TITLE_FONT       nanoplex32
#define TUTORIAL_TITLE_POSY       20

#define TUTORIAL_BODY_FONT        thintel32
// #define TUTORIAL_BODY_POSY        (TUTORIAL_TITLE_POSY + TUTORIAL_TITLE_FONT->line_height/2 + 30)
#define TUTORIAL_BODY_LINE_HEIGHT (TUTORIAL_BODY_FONT->line_height)

#define TUTORIAL_BUTTON_FONT       thintel32
#define TUTORIAL_BUTTON_RADIUS     18
#define TUTORIAL_BUTTON1_CENTERX   70
#define TUTORIAL_BUTTON2_CENTERX   250
#define TUTORIAL_BUTTON_CENTERY    195
#define TUTORIAL_BUTTON_LABEL_POSY (TUTORIAL_BUTTON_CENTERY + TUTORIAL_BUTTON_RADIUS + TUTORIAL_BUTTON_FONT->line_height/2)

// One entry per screen, in display order. Add new screens above TUTORIAL_SCREEN_COUNT,
// then add a matching draw_screen_*() function and switch case in tutorial.c.
enum tutorial_screen {
	TUTORIAL_SCREEN_WELCOME = 0,
	TUTORIAL_SCREEN_MATRIX,
	TUTORIAL_SCREEN_KNOB,
	TUTORIAL_SCREEN_MENU,
	TUTORIAL_SCREEN_MENU_01,
	TUTORIAL_SCREEN_MENU_02,
	TUTORIAL_SCREEN_QR_VIA,
	TUTORIAL_SCREEN_QR_WEB,
	TUTORIAL_SCREEN_COUNT
};

bool tutorial_is_active(void);
bool tutorial_matrix_demo_is_active(void); // true only on TUTORIAL_SCREEN_MATRIX - lets process_record_display() still light up real keypresses there
void tutorial_start(void); // shows screen 0 - only call once eepdata is loaded and the display is initialized
// false = Button 1 (Cancel on screen 0, Prev after), true = Button 2 (Next).
// Call on both press AND release (`pressed`) - TUTORIAL_SCREEN_MENU needs to
// see both buttons held together to pass its "press 2 buttons at once" practice.
void tutorial_button_action(bool is_button2, bool pressed);
void tutorial_knob_rotated(void); // call on every encoder tick while active; no-op except on TUTORIAL_SCREEN_KNOB
