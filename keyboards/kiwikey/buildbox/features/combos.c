#include "quantum.h"
#include "buildbox.h" // KC_BUTTON_1/2

#if defined(QUANTUM_PAINTER_ENABLE)

#include "display/defines.h"
#include "display/qp_graphics.h"   // display_is_asleep(), ui_get_mode()
#include "display/widgets/qp_menu.h" // menu_init()

enum combo_events {
	COMBO_MENU_TOGGLE,
};

const uint16_t PROGMEM menu_toggle_combo[] = {KC_BUTTON_1, KC_BUTTON_2, COMBO_END};

combo_t key_combos[] = {
	[COMBO_MENU_TOGGLE] = COMBO(menu_toggle_combo, KC_NO),
};

// Only let the chord arm from the idle screen (UI_MODE_IDLE - not the menu,
// Breakout, tutorial or screensaver). Same reasoning for display_is_asleep(): while
// asleep, the combo engine buffering Button 1/2 (waiting up to COMBO_TERM to
// see if the other follows) delays their arrival at process_record_display()
// past the point where its wake-up swallow check still sees the display as
// asleep (last_matrix_activity_trigger() already woke it by then), so the
// "first press only wakes" behavior gets skipped and the button's normal
// action (layer prev/next) fires too. Everywhere this returns false, the
// combo engine never buffers these keys at all, so Button 1/2 fall straight
// through to process_record_display() exactly as they do today, with no
// added latency.
bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
	return ui_get_mode() == UI_MODE_IDLE && !display_is_asleep();
}

void process_combo_event(uint16_t combo_index, bool pressed) {
	if (combo_index == COMBO_MENU_TOGGLE && pressed) {
		menu_init();
	}
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
