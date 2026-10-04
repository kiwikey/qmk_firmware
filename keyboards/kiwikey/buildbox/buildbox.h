#pragma once

#include "quantum.h"

// Keyboard-level keycodes live in QK_KB_0.. (SAFE_RANGE = QK_USER_0 is reserved
// for keymap-level ones). VIA's "customKeycodes" (buildbox_via.json) map to
// QK_KB_0, QK_KB_1, ... in list order, so this enum and that list must stay
// in the same order.
enum custom_keycodes {
	KC_BUTTON_1 = QK_KB_0, // 0x7E00
	KC_BUTTON_2,           // 0x7E01
	KC_WIN_CUT,            // Ctrl+X
	KC_WIN_COPY,           // Ctrl+C
	KC_WIN_PASTE,          // Ctrl+V
	KC_WIN_DESKTOP,        // Win+D
	KC_WIN_SNIP,           // Win+Shift+S
	KC_MAC_CUT,            // Cmd+X
	KC_MAC_COPY,           // Cmd+C
	KC_MAC_PASTE           // Cmd+V
};
