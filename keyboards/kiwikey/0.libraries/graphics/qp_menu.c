#if defined(QUANTUM_PAINTER_ENABLE)

#include "qp_graphics.h"
#include "qp_menu.h"

extern painter_device_t my_display;

extern painter_font_handle_t  thintel;
extern painter_font_handle_t  thintel50;
extern painter_font_handle_t  arial30;

uint8_t current_menu = NOT_IN_MENU;
uint8_t menu_execute = 0; // 0 means no "menu line" is being activated/chosen
// static uint8_t menu_cursor = MAINMENU_1STLINE_POS;

void menu_init(void) {
    qp_rect(my_display, 0, 0, ST7789_WIDTH, ST7789_HEIGHT, HSV_BLACK, true);
	// Menu Title "SETTING"
	qp_drawtext(my_display, 50, 0, arial30, "SETTINGS");
	// menu_printlist();               // Print the menu list
	// menu_set_cursor(menu_cursor);   // Print the cursor
	// menu_quick_view(menu_cursor);   // Print the default quick view
}

// void menu_printlist(void) { // Print the menu list, total MAINMENU_LINESPERPAGE lines
	// Clear the old list
	// oled_set_cursor(0, MAINMENU_1STLINE_POS);
	// for (uint8_t i = 0; i < MAINMENU_LINESPERPAGE; i++) oled_advance_page(true);
	
	// Print the list based on menu_cursor
	// oled_set_cursor(0, MAINMENU_1STLINE_POS);
	// if (menu_cursor <= MAINMENU_LINESPERPAGE) {
		// for (uint8_t i = 0; i < MAINMENU_LINESPERPAGE; i++) {
			// qp_drawtext(my_display, 50, 0, arial30, menu_list[i]);
		// }
	// }
	// else if (menu_cursor > MAINMENU_LINESPERPAGE) {
		// for (uint8_t i = MAINMENU_LINESPERPAGE; i < 9; i++) {
			// oled_write_ln(menu_list[i], false);
		// }
	// }
// }

#endif // defined(QUANTUM_PAINTER_ENABLE)
