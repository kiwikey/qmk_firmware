#include "tutorial.h"

#if TUTORIAL_ENABLE && defined(QUANTUM_PAINTER_ENABLE)

#include <lib/lib8tion/lib8tion.h> // cos8/sin8, for the live knob dot
#include "features/eeprom_custom.h"
#include "sensor/as5600.h" // magnetic_encoder
#include "sensor/sensors_handler.h" // accumulator
#include "display/qp_graphics.h"
#include "display/qp_includes.h"
#include "display/qp_custom_api.h"
#include "display/defines.h"
#include "display/widgets/qp_menu.h" // eeprom_update_custom()
#include "display/widgets/qp_widget_knob.h" // widget_knob_draw_ring(), WIDGET_KNOB_DOT_*
#include "display/widgets/qp_widget_matrix.h" // widget_matrix_init(), widget_matrix_keymap_render()
#include "display/widgets/qp_widget_layer.h" // widget_layer_init(), widget_layer_render_layername()

static bool active  = false;
static uint8_t current_screen = TUTORIAL_SCREEN_WELCOME;

// Live dot on TUTORIAL_SCREEN_KNOB, drawn at the same WIDGET_KNOB_CENTERX/Y
// position/size as the idle screen's knob widget - deliberately not reusing
// widget_knob_draw_dot() itself though (it tracks its own last_drawn_x/y),
// just duplicated here flat/simple instead.
static int16_t knob_dot_x = -1, knob_dot_y = -1; // -1 = nothing drawn yet this screen visit

static void draw_knob_dot(void) {
	int angle = 255 - (magnetic_encoder.new_angle >> 4);
	int16_t x = WIDGET_KNOB_CENTERX + (WIDGET_KNOB_DOT_ORBIT_RADIUS * (cos8(angle)-128)) / 128;
	int16_t y = WIDGET_KNOB_CENTERY - (WIDGET_KNOB_DOT_ORBIT_RADIUS * (sin8(angle)-128)) / 128;

	if (knob_dot_x >= 0) { // erase the previous dot, if any was drawn since this screen was last shown
		qp_circle(my_display, knob_dot_x, knob_dot_y, WIDGET_KNOB_DOT_SIZE, GLOBAL_BG_COLOR, WIDGET_KNOB_DOT_IS_FILLED);
	}
	qp_circle(my_display, x, y, WIDGET_KNOB_DOT_SIZE, WIDGET_KNOB_DOT_COLOR, WIDGET_KNOB_DOT_IS_FILLED);
	knob_dot_x = x;
	knob_dot_y = y;
}


static void draw_screen_welcome(void) {
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 20, TUTORIAL_TITLE_FONT, "QUICK TUTORIAL", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 90, TUTORIAL_BODY_FONT, "We will walk through", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 90 + TUTORIAL_BODY_LINE_HEIGHT, TUTORIAL_BODY_FONT, "the BuildBox user guide.", HSV_WHITE, GLOBAL_BG_COLOR);

	qp_circle(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Cancel", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_circle(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Start", HSV_WHITE, GLOBAL_BG_COLOR);
}

// Just the real widget matrix + layer name, as they'd normally appear on the
// idle screen (see ui_refresh() in qp_graphics.c) - no separate title text, so
// nothing collides with widget_layer_init()'s own layer-name box at the top.
// Matrix occupies x:5-208 (WIDGET_MATRIX_POSX/WIDTH), so the lone Next button
// at TUTORIAL_BUTTON2_CENTERX (250) sits clear of it on the right.
static void draw_screen_matrix(void) {
	widget_matrix_init();
	widget_layer_render_layername(0);
	widget_matrix_keymap_render(0);

	qp_drawtext_recolor(my_display, 250,  3, TUTORIAL_BODY_FONT, "Layer", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display, 240, 60, TUTORIAL_BODY_FONT, "Keymap", HSV_WHITE, GLOBAL_BG_COLOR);

	qp_line(my_display, WIDGET_LAYER_WIDTH + 15, 15, 245, 15, HSV_CYAN);
	qp_line(my_display, 270,  85, 270, 120, HSV_CYAN);
	qp_line(my_display, 220, 120, 270, 120, HSV_CYAN);
	qp_circle(my_display, WIDGET_LAYER_WIDTH + 15, 15, 5, HSV_CYAN, true);
	qp_circle(my_display, 220, 120, 5, HSV_CYAN, true);

	qp_circle(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Next", HSV_WHITE, GLOBAL_BG_COLOR);
}

static void draw_screen_knob(void) {
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 20, TUTORIAL_TITLE_FONT, "ROTARY KNOB", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display, 20, 30 + TUTORIAL_BODY_LINE_HEIGHT,   TUTORIAL_BODY_FONT, "> Adjust volume", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display, 20, 30 + TUTORIAL_BODY_LINE_HEIGHT*2, TUTORIAL_BODY_FONT, "> Scroll up/down", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display, 20, 30 + TUTORIAL_BODY_LINE_HEIGHT*3, TUTORIAL_BODY_FONT, "    or left/right", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display, 20, 30 + TUTORIAL_BODY_LINE_HEIGHT*4, TUTORIAL_BODY_FONT, "> Navigate in Menu", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor(my_display, 20, 30 + TUTORIAL_BODY_LINE_HEIGHT*5, TUTORIAL_BODY_FONT, "    & change settings", HSV_WHITE, GLOBAL_BG_COLOR);

	widget_knob_draw_ring(WIDGET_KNOB_CENTERX, WIDGET_KNOB_CENTERY, WIDGET_KNOB_RADIUS);
	knob_dot_x = -1; // screen was just cleared - nothing to erase yet
	draw_knob_dot();

	qp_circle(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Prev", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_circle(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Next", HSV_WHITE, GLOBAL_BG_COLOR);
}

// TUTORIAL_SCREEN_MENU: true once the user has pressed both buttons together
// this screen visit - gates its Prev/Next circles (see draw_screen_menu()/
// draw_screen_menu_success() below) and the button-hold tracking they need.
static bool menu_combo_done   = false;
static bool button1_held      = false;
static bool button2_held      = false;

// Prev/Next circles start hidden - only draw_screen_menu_success() reveals them,
// once the user actually practices the real "press 2 buttons at once" combo.
static void draw_screen_menu(void) {
	menu_combo_done = false;
	button1_held    = false;
	button2_held    = false;

	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, TUTORIAL_TITLE_POSY, TUTORIAL_TITLE_FONT, "BUILDBOX MENU", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 50, TUTORIAL_BODY_FONT, "For settings, and more", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 60 + TUTORIAL_BODY_LINE_HEIGHT*2, TUTORIAL_BODY_FONT, "To enter Menu,", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 60 + TUTORIAL_BODY_LINE_HEIGHT*3, TUTORIAL_BODY_FONT, "press 2 buttons at once", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 70 + TUTORIAL_BODY_LINE_HEIGHT*4, TUTORIAL_BODY_FONT, "Try it!", HSV_WHITE, GLOBAL_BG_COLOR);
}

// Called once button1_held && button2_held on TUTORIAL_SCREEN_MENU: swaps the
// "Try it!" line for "You are a genius!" and reveals the Prev/Next circles.
static void draw_screen_menu_success(void) {
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 70 + TUTORIAL_BODY_LINE_HEIGHT*4, TUTORIAL_BODY_FONT, "You are a genius!", HSV_WHITE, GLOBAL_BG_COLOR);

	qp_circle(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Prev", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_circle(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Next", HSV_WHITE, GLOBAL_BG_COLOR);

	qp_flush(my_display); // drawn outside draw_screen(), so it needs its own flush
}

// TUTORIAL_SCREEN_MENU_01: TUTORIAL_MENU01_LINE_COUNT example menu lines the
// user can practice moving a cursor between with the knob - same look as the
// real Settings Menu's cursor (ico16_arrow_right, see menu_set_cursor() in qp_menu.c).
#define TUTORIAL_MENU01_ICON_POSX    20
#define TUTORIAL_MENU01_TEXT_POSX    40
#define TUTORIAL_MENU01_VALUE_POSX   200
#define TUTORIAL_MENU01_LINE_POSY(i) (80 + (i) * 22)
#define TUTORIAL_MENU01_LINE_COUNT   3
static uint8_t menu01_cursor      = 0;   // which example line is currently selected - shared with TUTORIAL_SCREEN_MENU_02 below, since the 2 screens are never shown at once
static uint8_t menu01_cursor_last = 255; // 255 = nothing drawn yet this screen visit
static const char * const menu01_line_text[TUTORIAL_MENU01_LINE_COUNT] = {
	"BRIGHTNESS",
	"SENSITIVITY",
	"RGB MODE",
};

static void draw_menu01_cursor(void) {
	if (menu01_cursor_last != 255) { // erase the old arrow, if any was drawn since this screen was last shown
		qp_rect(my_display,
		        TUTORIAL_MENU01_ICON_POSX, TUTORIAL_MENU01_LINE_POSY(menu01_cursor_last),
		        TUTORIAL_MENU01_ICON_POSX + 8 - 1, TUTORIAL_MENU01_LINE_POSY(menu01_cursor_last) + 16 - 1,
		        GLOBAL_BG_COLOR, true);
	}
	qp_drawimage_recolor(my_display, TUTORIAL_MENU01_ICON_POSX, TUTORIAL_MENU01_LINE_POSY(menu01_cursor), ico16_arrow_right, GLOBAL_THEME_COLOR, GLOBAL_BG_COLOR);
	menu01_cursor_last = menu01_cursor;
}

static void draw_screen_menu_01(void) {
	accumulator = 0; // clear this to avoid a "weird cursor jump" from rotation on a previous screen

	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, TUTORIAL_TITLE_POSY, TUTORIAL_TITLE_FONT, "BUILDBOX MENU", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 50, TUTORIAL_BODY_FONT, "Rotate knob to navigate", HSV_WHITE, GLOBAL_BG_COLOR);

	menu01_cursor      = 0;
	menu01_cursor_last = 255; // screen was just cleared - nothing to erase yet
	for (uint8_t i = 0; i < TUTORIAL_MENU01_LINE_COUNT; i++) {
		qp_drawtext_recolor(my_display, TUTORIAL_MENU01_TEXT_POSX, TUTORIAL_MENU01_LINE_POSY(i), TUTORIAL_BODY_FONT, menu01_line_text[i], HSV_WHITE, GLOBAL_BG_COLOR);
	}
	draw_menu01_cursor();

	qp_circle(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Prev", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_circle(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Next", HSV_WHITE, GLOBAL_BG_COLOR);
}

#define TUTORIAL_MENU02_LINE_COUNT   4 // BRIGHTNESS, SENSITIVITY, RGB MODE, QUIT DEMO
#define TUTORIAL_MENU02_VALUE_COUNT  3 // each changeable item cycles through 3 example values
static bool    menu02_selected                                     = false; // true = the item under the cursor is "active" right now
static bool    menu02_demo_running                                 = true;  // false once QUIT DEMO is selected
static uint8_t menu02_value_index[TUTORIAL_MENU02_LINE_COUNT - 1]   = { 1, 1, 2 }; // indexes into menu02_value_options - starts at 80%, MEDIUM, MODE 3

static const char * const menu02_line_text[TUTORIAL_MENU02_LINE_COUNT] = {
	"BRIGHTNESS",
	"SENSITIVITY",
	"RGB MODE",
	"QUIT DEMO",
};
static const char * const menu02_value_options[TUTORIAL_MENU02_LINE_COUNT - 1][TUTORIAL_MENU02_VALUE_COUNT] = {
	{ " 50%"  ,  " 80%" ,  "100%"  },
	{ "LOW   ", "MEDIUM", "HIGH  " },
	{ "MODE 1", "MODE 2", "MODE 3" },
};

static void draw_menu02_value(uint8_t row, bool active) {
	const char *value = menu02_value_options[row][menu02_value_index[row]];
	if (active) {
		qp_drawtext_recolor(my_display, TUTORIAL_MENU01_VALUE_POSX, TUTORIAL_MENU01_LINE_POSY(row), TUTORIAL_BODY_FONT, value, GLOBAL_THEME_COLOR, GLOBAL_BG_COLOR);
	} else {
		qp_drawtext_recolor(my_display, TUTORIAL_MENU01_VALUE_POSX, TUTORIAL_MENU01_LINE_POSY(row), TUTORIAL_BODY_FONT, value, HSV_WHITE, GLOBAL_BG_COLOR);
	}
}

static void draw_menu02_buttons(const char *label1, const char *label2) {
	qp_circle(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, label1, HSV_WHITE, GLOBAL_BG_COLOR);
	qp_circle(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, label2, HSV_WHITE, GLOBAL_BG_COLOR);
}

static void draw_screen_menu_02(void) {
	accumulator            = 0; // clear this to avoid a "weird cursor jump" from rotation on a previous screen
	menu02_selected        = false;
	menu02_demo_running    = true;
	menu02_value_index[0]  = 1; // 80%
	menu02_value_index[1]  = 1; // MEDIUM
	menu02_value_index[2]  = 2; // MODE 3

	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, TUTORIAL_TITLE_POSY, TUTORIAL_TITLE_FONT, "BUILDBOX MENU", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 50, TUTORIAL_BODY_FONT, "Button: Left = Exit, Right = OK", HSV_WHITE, GLOBAL_BG_COLOR);

	menu01_cursor      = 0;   // shared cursor var/drawer with TUTORIAL_SCREEN_MENU_01
	menu01_cursor_last = 255; // screen was just cleared - nothing to erase yet
	for (uint8_t i = 0; i < TUTORIAL_MENU02_LINE_COUNT; i++) {
		qp_drawtext_recolor(my_display, TUTORIAL_MENU01_TEXT_POSX, TUTORIAL_MENU01_LINE_POSY(i), TUTORIAL_BODY_FONT, menu02_line_text[i], HSV_WHITE, GLOBAL_BG_COLOR);
	}
	for (uint8_t i = 0; i < TUTORIAL_MENU02_LINE_COUNT - 1; i++) { // QUIT DEMO has no value - same as a real ischangeable=false item
		draw_menu02_value(i, false);
	}
	draw_menu01_cursor();

	draw_menu02_buttons("Exit", "OK");
}

// TUTORIAL_SCREEN_QR_VIA/_WEB: a QR code placeholder box - actual QR image to
// be dropped in later (qp_drawimage() inside the box below). Both screens are
// otherwise plain title+text+buttons like every other simple screen here.
#define TUTORIAL_QR_SIZE  120
#define TUTORIAL_QR_POSX  ((ST7789_WIDTH - TUTORIAL_QR_SIZE) / 2)
#define TUTORIAL_QR_POSY  60

static void draw_screen_qr_via(void) {
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, TUTORIAL_TITLE_POSY, TUTORIAL_TITLE_FONT, "KEYMAP APP", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 40, TUTORIAL_BODY_FONT, "www.usevia.app", HSV_WHITE, GLOBAL_BG_COLOR);

	// TODO: qp_drawimage(my_display, TUTORIAL_QR_POSX, TUTORIAL_QR_POSY, <qr image>) once the QR code asset exists
	qp_rect(my_display, TUTORIAL_QR_POSX, TUTORIAL_QR_POSY, TUTORIAL_QR_POSX + TUTORIAL_QR_SIZE, TUTORIAL_QR_POSY + TUTORIAL_QR_SIZE, HSV_WHITE, false);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, TUTORIAL_QR_POSY + TUTORIAL_QR_SIZE/2, TUTORIAL_BODY_FONT, "QR CODE", HSV_WHITE, GLOBAL_BG_COLOR);

	qp_circle(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Prev", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_circle(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Next", HSV_WHITE, GLOBAL_BG_COLOR);
}

static void draw_screen_qr_web(void) {
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, TUTORIAL_TITLE_POSY, TUTORIAL_TITLE_FONT, "MORE INFO", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, 40, TUTORIAL_BODY_FONT, "www.BuildBox.com", HSV_WHITE, GLOBAL_BG_COLOR);

	// TODO: qp_drawimage(my_display, TUTORIAL_QR_POSX, TUTORIAL_QR_POSY, <qr image>) once the QR code asset exists
	qp_rect(my_display, TUTORIAL_QR_POSX, TUTORIAL_QR_POSY, TUTORIAL_QR_POSX + TUTORIAL_QR_SIZE, TUTORIAL_QR_POSY + TUTORIAL_QR_SIZE, HSV_WHITE, false);
	qp_drawtext_recolor_center(my_display, ST7789_WIDTH/2, TUTORIAL_QR_POSY + TUTORIAL_QR_SIZE/2, TUTORIAL_BODY_FONT, "QR CODE", HSV_WHITE, GLOBAL_BG_COLOR);

	qp_circle(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON1_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Prev", HSV_WHITE, GLOBAL_BG_COLOR);
	qp_circle(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_CENTERY, TUTORIAL_BUTTON_RADIUS, GLOBAL_THEME_COLOR, true);
	qp_drawtext_recolor_center(my_display, TUTORIAL_BUTTON2_CENTERX, TUTORIAL_BUTTON_LABEL_POSY, TUTORIAL_BUTTON_FONT, "Finish", HSV_WHITE, GLOBAL_BG_COLOR);
}

// Add new screens above (and to `enum tutorial_screen` in tutorial.h), then add a case here.
static void draw_screen(void) {
	qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, GLOBAL_BG_COLOR, true); // Clear screen

	switch (current_screen) {
		case TUTORIAL_SCREEN_WELCOME:  draw_screen_welcome();  break;
		case TUTORIAL_SCREEN_MATRIX:   draw_screen_matrix();   break;
		case TUTORIAL_SCREEN_KNOB:     draw_screen_knob();     break;
		case TUTORIAL_SCREEN_MENU:     draw_screen_menu();     break;
		case TUTORIAL_SCREEN_MENU_01:  draw_screen_menu_01();  break;
		case TUTORIAL_SCREEN_MENU_02:  draw_screen_menu_02();  break;
		case TUTORIAL_SCREEN_QR_VIA:   draw_screen_qr_via();   break;
		case TUTORIAL_SCREEN_QR_WEB:   draw_screen_qr_web();   break;
	}

	qp_flush(my_display);
}

static void tutorial_finish(void) {
	active  = false;
	eepdata.unbox_tutorial = 0;
	eeprom_update_custom(); // persist so it doesn't show again on next boot
	accumulator = 0; // discard rotation accumulated while the tutorial's knob screen was up
	ui_refresh();
}

bool tutorial_is_active(void) {
	return active;
}

bool tutorial_matrix_demo_is_active(void) {
	return active && current_screen == TUTORIAL_SCREEN_MATRIX;
}

void tutorial_start(void) {
	active  = true;
	current_screen = TUTORIAL_SCREEN_WELCOME;
	draw_screen();
}

void tutorial_button_action(bool is_button2, bool pressed) {
	if (!active) return;

	if (is_button2) button2_held = pressed; else button1_held = pressed;
	if (!pressed) return; // navigation/practice only react to the press, not the release

	// TUTORIAL_SCREEN_MENU: Button 1/2 don't navigate until the user has
	// actually practiced pressing both together - a single button alone does nothing.
	if (current_screen == TUTORIAL_SCREEN_MENU && !menu_combo_done) {
		if (button1_held && button2_held) {
			menu_combo_done = true;
			draw_screen_menu_success();
		}
		return;
	}

	// TUTORIAL_SCREEN_MENU_02 demo - see the big comment above menu02_selected
	// for the full behavior. Once something is selected, EITHER button
	// deselects it (real Menu: both Button 1/2 call menu_submenu_exit() while
	// menu_state == SUB_MENU, see process_record_display() in qp_graphics.c) -
	// Right/OK only *selects* while nothing is selected yet.
	if (current_screen == TUTORIAL_SCREEN_MENU_02 && menu02_demo_running) {
		if (menu02_selected) {
			menu02_selected = false;
			draw_menu02_value(menu01_cursor, false);
			qp_flush(my_display);
		} else if (is_button2) { // OK: select the item under the cursor
			if (menu01_cursor == TUTORIAL_MENU02_LINE_COUNT - 1) { // QUIT DEMO
				menu02_demo_running = false;
				draw_menu02_buttons("Prev", "Next");
				qp_flush(my_display);
			} else {
				menu02_selected = true;
				draw_menu02_value(menu01_cursor, true);
				qp_flush(my_display);
			}
		}
		// Exit while nothing is selected: no-op
		return;
	}

	if (is_button2) { // Next
		if (current_screen + 1 >= TUTORIAL_SCREEN_COUNT) {
			tutorial_finish(); // no screen after this one (yet) - finishing the last one ends the tutorial
			return;
		}
		current_screen++;
	} else { // Cancel (screen 0) / Prev (every screen after)
		if (current_screen == TUTORIAL_SCREEN_WELCOME) {
			tutorial_finish();
			return;
		}
		current_screen--;
	}
	draw_screen();
}

void tutorial_knob_rotated(void) {
	if (!active) return;

	if (current_screen == TUTORIAL_SCREEN_KNOB) {
		draw_knob_dot();
		qp_flush(my_display);
		return;
	}

	if (current_screen == TUTORIAL_SCREEN_MENU_02 && menu02_selected) {
		// An item is "active" - the knob cycles its example value instead of
		// moving the cursor, same MENU_STEP_SIZE-per-detent consumption as below.
		bool moved = false;
		while (accumulator >= MENU_STEP_SIZE) {
			menu02_value_index[menu01_cursor] = (menu02_value_index[menu01_cursor] + 1) % TUTORIAL_MENU02_VALUE_COUNT;
			accumulator -= MENU_STEP_SIZE;
			moved = true;
		}
		while (accumulator <= -MENU_STEP_SIZE) {
			menu02_value_index[menu01_cursor] = (menu02_value_index[menu01_cursor] + TUTORIAL_MENU02_VALUE_COUNT - 1) % TUTORIAL_MENU02_VALUE_COUNT;
			accumulator += MENU_STEP_SIZE;
			moved = true;
		}
		if (moved) {
			draw_menu02_value(menu01_cursor, true);
			qp_flush(my_display);
		}
		return;
	}

	if (current_screen == TUTORIAL_SCREEN_MENU_01 || current_screen == TUTORIAL_SCREEN_MENU_02) {
		uint8_t line_count = (current_screen == TUTORIAL_SCREEN_MENU_02) ? TUTORIAL_MENU02_LINE_COUNT : TUTORIAL_MENU01_LINE_COUNT;

		// Same MENU_STEP_SIZE-per-detent consumption as the real menu's list
		// navigation (see the MAIN_MENU/SUB_MENU branch in magnetic_encoder_update_kb()).
		bool moved = false;
		while (accumulator >= MENU_STEP_SIZE) {
			menu01_cursor = (menu01_cursor + 1) % line_count; // CW = down
			accumulator -= MENU_STEP_SIZE;
			moved = true;
		}
		while (accumulator <= -MENU_STEP_SIZE) {
			menu01_cursor = (menu01_cursor + line_count - 1) % line_count; // CCW = up
			accumulator += MENU_STEP_SIZE;
			moved = true;
		}
		if (moved) {
			draw_menu01_cursor();
			qp_flush(my_display);
		}
	}
}

#else // !(TUTORIAL_ENABLE && defined(QUANTUM_PAINTER_ENABLE))

bool tutorial_is_active(void) { return false; }
bool tutorial_matrix_demo_is_active(void) { return false; }
void tutorial_start(void) {}
void tutorial_button_action(bool is_button2, bool pressed) {}
void tutorial_knob_rotated(void) {}

#endif
