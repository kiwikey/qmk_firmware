#pragma once

#define I2C1_SDA_PIN GP2
#define I2C1_SCL_PIN GP3

#define QUANTUM_PAINTER_DISPLAY_TIMEOUT 0  // e.g. 60s, or 0 to disable

// Syncs master's last_matrix/encoder/pointing_device activity timestamps down to the slave
// (see quantum/split_common/transactions.c's activity_handlers_master/_slave). Without this,
// each half's last_input_activity_elapsed() only reflects that half's own local matrix, so
// they can independently and incorrectly disagree about idle state - the master (which
// receives the slave's matrix over the standard split sync) has the only combined, accurate
// view. modules/kiwikey/kiwikey_oled's idle-mode timeout (see lib/oled_render.c) reads
// last_input_activity_elapsed() directly, so this fixes it on both halves for free.
#define SPLIT_ACTIVITY_ENABLE

// The lock indicator widget (modules/kiwikey/kiwikey_oled) lives on the slave screen and
// reads host_keyboard_led_state() - which, like activity timestamps above, is otherwise
// only accurate on the master (only it talks to the host over USB). This syncs the real
// caps/num/scroll lock state down to the slave so it isn't stuck showing stale/default state.
#define SPLIT_LED_STATE_ENABLE