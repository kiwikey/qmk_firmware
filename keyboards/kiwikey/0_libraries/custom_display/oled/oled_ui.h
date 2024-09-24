#ifndef OLED_UI
#define OLED_UI

#include "quantum.h"

// Sub UI is the bottom half of OLED screen, shows WPM chart, RGB infos,...
// sub_ui_mode = 0 : WPM graph
//               1 : RGB control
//               2 : Media control (to do)
#define SUB_UI_DISPLAY_ROW 4
#define SUB_UI_TIMEOUT 2000 // ms

// Display configuration for stats (by COL/ROW count, not by pixel)
#define STATS_DISPLAY_COL 0
#define STATS_DISPLAY_ROW 1 // Be careful setting this to 0 (zero)
/*** Example of Stats display ***
    LAYER 0  WPM 123
    
    RGB #03  * 55%
********************************/

static const char * const layer_name[DYNAMIC_KEYMAP_LAYER_COUNT] = {
	// "Default",
	// " Media ",
	// " Mouse ",
	// "User #1",
	// "User #2",
	// "  RGB  "
	"DEFAULT", // 0
	" MEDIA ", // 1
	" MOUSE ", // 2
	"USER #1", // 3
	"USER #2", // 4
	"  RGB  "  // 5
};

extern uint8_t rgb_val;
extern uint8_t sub_ui_mode;

void ui_refresh(void);
void render_ui_frame(void);
void render_stats(void);
void sub_ui_clear(void);
void render_subui_rgbcontrol(void);

#endif /* OLED_UI */
