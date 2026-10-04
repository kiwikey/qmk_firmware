#pragma once

/*
 * BuildBox's own settings, stored in the VIA "custom config" EEPROM block
 * (VIA_EEPROM_CUSTOM_CONFIG_ADDR, VIA_EEPROM_CUSTOM_CONFIG_SIZE bytes - config.h).
 * Everything that reads or writes that block goes through this module:
 *   eeprom_custom_load()     - once at boot (keyboard_post_init_kb(), buildbox.c)
 *   eeprom_custom_save()     - menu exit, VIA "save", tutorial finish, ...
 *   eeprom_custom_validate() - clamps every field into its valid range
 */

#include "quantum.h"
#include "eeprom.h"
#include "nvm_eeprom_via_internal.h"
#include "nvm_eeprom_eeconfig_internal.h"

// Size of the per-layer arrays below. Deliberately a fixed number, not
// DYNAMIC_KEYMAP_LAYER_COUNT: tying them to the layer count would shift every
// later field's EEPROM offset the moment the layer count changes. Raising this
// is a layout change (see EEPROM_CUSTOM_LAYOUT_VERSION).
#define EEPROM_CUSTOM_MAX_LAYERS 4
_Static_assert(DYNAMIC_KEYMAP_LAYER_COUNT <= EEPROM_CUSTOM_MAX_LAYERS, "DYNAMIC_KEYMAP_LAYER_COUNT exceeds EEPROM_CUSTOM_MAX_LAYERS - raise it (layout change)");

// Stored in the struct's last byte and compared at boot (eeprom_custom_load()):
// a mismatch resets everything to eepdata_default. Bump it whenever
// the layout changes (a field added, removed, resized or moved), so a board with
// the old layout gets one clean reset instead of reading old bytes at new offsets.
// History: was a fixed "checksum = 7" sentinel, so 7 is the current layout.
#define EEPROM_CUSTOM_LAYOUT_VERSION 7

typedef struct {
	uint8_t active_layer;
	uint8_t display_bootanim;
	uint8_t display_timeout;
	uint8_t display_brightness;
	uint8_t lighting_layers; // 0 = OFF; 1 = ON
	uint8_t layer_hue[EEPROM_CUSTOM_MAX_LAYERS];
	uint8_t layer_sat[EEPROM_CUSTOM_MAX_LAYERS];
	uint8_t knob_effect;
	uint8_t knob_func;
	uint8_t theme_hue;
	uint8_t knob_sensitivity; // LOW/MEDIUM/HIGH index, see KNOB_SENSITIVITY_* and knob_sensitivity_step[] (sensors_handler.h)
	uint8_t unbox_tutorial;   // 1 = show the first-boot tutorial (display/widgets/tutorial.c), 0 = already shown/dismissed
	uint8_t screensaver_effect; // 0 = OFF, 1.. = selected effect; see SCREEN_SAVER_MAXITEMS/screen_saver_effect_list[] (display/widgets/qp_menu.h)
	uint8_t layer_icon[EEPROM_CUSTOM_MAX_LAYERS]; // pool index into layer_icon_pool_icon() (display/widgets/qp_widget_layer.c) - picked in the "LAYERS CONFIG" menu
	// New fields go here, directly above layout_version - never in the middle (shifts every later field)
	uint8_t layout_version; // EEPROM_CUSTOM_LAYOUT_VERSION - must stay the last field
} eeprom_custom_t;

extern eeprom_custom_t       eepdata;          // live settings - read/written directly everywhere
extern const eeprom_custom_t eepdata_default;  // factory defaults (eeprom_custom.c)

void eeprom_custom_load(void);     // read from EEPROM; reset to defaults on a layout_version mismatch; then validate
void eeprom_custom_save(void);     // write eepdata back (only changed bytes are actually written)
void eeprom_custom_reset(void);    // eepdata = defaults, and save
bool eeprom_custom_validate(void); // clamp every out-of-range field back to its default - returns true if anything changed
