#include "quantum.h"

#if defined(QUANTUM_PAINTER_ENABLE)

#include "display/defines.h"
#include "display/widgets/qp_menu.h"
#include "display/widgets/qp_widget_breakout.h"
#include "display/widgets/tutorial.h"

enum combo_events {
    COMBO_MENU_TOGGLE,
};

const uint16_t PROGMEM menu_toggle_combo[] = {KC_BUTTON_1, KC_BUTTON_2, COMBO_END};

combo_t key_combos[] = {
    [COMBO_MENU_TOGGLE] = COMBO(menu_toggle_combo, KC_NO),
};

// Only let the chord arm from the idle screen: Breakout sets menu_state to
// NOT_IN_MENU while it plays (see action_breakout() in qp_menu.c), and the
// tutorial does too (see tutorial.c), so both need their own check here, not
// just a menu_state comparison. Everywhere else, returning false here means
// the combo engine never buffers these keys at all, so Button 1/2 fall
// straight through to process_record_display() exactly as they do today,
// with no added latency.
bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    return menu_state == NOT_IN_MENU && !breakout_is_active() && !tutorial_is_active();
}

void process_combo_event(uint16_t combo_index, bool pressed) {
    if (combo_index == COMBO_MENU_TOGGLE && pressed) {
        menu_init();
    }
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
