#pragma once

/*
 * via_extra - the BuildBox <-> web app protocol for per-layer names and icons, device settings and macro names.
 * Spec (single source of truth, owned by the web-app side):
 *     D:\Claude_Software\buildbox-protocol\PROTOCOL.md   (this code follows draft v1, revision 7)
 *
 * It rides on the VIA raw HID channel with its own command byte (VIA_EXTRA_CMD). The reply is the
 * request buffer modified in place, so bytes 0 .. 2+ARGS_LEN come back unchanged (the spec's echo rule).
 *
 * Storage:
 *   - names : a dedicated EEPROM region at the very end of EEPROM (VIA_EXTRA_EEPROM_ADDR, config.h),
 *             carved out of the VIA macro buffer by lowering DYNAMIC_KEYMAP_EEPROM_MAX_ADDR.
 *             Survives reflashing and VIA's keymap/macro reset; wiped only by a full EEPROM erase.
 *   - macro names : a second region just below it (VIA_EXTRA_MACRO_EEPROM_ADDR), carved out the same way.
 *             They are separate from the macro content: VIA's macro commands never touch them.
 *   - icons : not stored here - they live in eepdata.layer_icon[] (features/eeprom_custom.h),
 *             the same values the LCD's "LAYERS CONFIG" menu picks. Protocol icon id = pool index + 1.
 */

#include "quantum.h"

#define VIA_EXTRA_CMD                0xE0
#define VIA_EXTRA_VERSION             1   // highest via_extra version this firmware speaks

// Operations (spec section 4)
#define VIA_EXTRA_OP_GET_CAPS         0x01
#define VIA_EXTRA_OP_GET_LAYER_NAME   0x10
#define VIA_EXTRA_OP_SET_LAYER_NAME   0x11
#define VIA_EXTRA_OP_GET_LAYER_ICON   0x20
#define VIA_EXTRA_OP_SET_LAYER_ICON   0x21
#define VIA_EXTRA_OP_SAVE             0x30
#define VIA_EXTRA_OP_CANCEL           0x31
#define VIA_EXTRA_OP_GET_STATE        0x41
#define VIA_EXTRA_OP_GET_MACRO_NAME   0x60
#define VIA_EXTRA_OP_SET_MACRO_NAME   0x61
#define VIA_EXTRA_OP_GET_LAYER_COUNT  0x70
#define VIA_EXTRA_OP_ADD_LAYER        0x71
#define VIA_EXTRA_OP_REMOVE_LAYER     0x72
#define VIA_EXTRA_OP_GET_SETTING      0x50
#define VIA_EXTRA_OP_SET_SETTING      0x51

// Setting ids for GET_SETTING / SET_SETTING (spec section 4a). Values are unsigned 16-bit, big-endian.
#define VIA_EXTRA_SETTING_SCREENSAVER_IDLE_MINUTES 0x01 // minutes, SCREENSAVER_IDLE_MIN_MIN..MAX (eeprom_custom.h)
#define VIA_EXTRA_SETTING_DIAL_SETTING_HOLD_MS     0x02 // milliseconds, exactly 1000, 1500, 2000, 2500 or 3000

// Status codes (spec section 5)
#define VIA_EXTRA_STATUS_OK                  0x00
#define VIA_EXTRA_STATUS_UNKNOWN_OP          0x01
#define VIA_EXTRA_STATUS_BAD_ARGS            0x02
#define VIA_EXTRA_STATUS_BUSY                0x03
#define VIA_EXTRA_STATUS_UNSUPPORTED_VERSION 0x04
#define VIA_EXTRA_STATUS_STORAGE_ERROR       0x05

// GET_CAPS "features" bits
#define VIA_EXTRA_FEATURE_NAMES       (1 << 0)
#define VIA_EXTRA_FEATURE_ICONS       (1 << 1)
#define VIA_EXTRA_FEATURE_GET_STATE   (1 << 2)
#define VIA_EXTRA_FEATURE_SETTINGS    (1 << 4) // bit 3 is reserved (layer-changed events), always 0
#define VIA_EXTRA_FEATURE_MACRO_NAMES (1 << 5)
#define VIA_EXTRA_FEATURE_LAYER_EDIT  (1 << 6) // add/remove layers (GET_LAYER_COUNT, ADD_LAYER, REMOVE_LAYER, CANCEL)

#define VIA_EXTRA_NAME_FIELD          16  // the name field is always 16 bytes on the wire
#define VIA_EXTRA_NAME_MAX            11  // characters accepted (what fits in the layer box next to its icon)

// Macro names: how many macros can be named, and how many characters the name may have. The name is
// drawn on the macro's key in the matrix widget, so the length is the label length that fits there.
#define VIA_EXTRA_MACRO_COUNT         DYNAMIC_KEYMAP_MACRO_COUNT
#define VIA_EXTRA_MACRO_NAME_MAX      10 // 2 lines of VIA_EXTRA_MACRO_LINE_CHARS on the key (50 px wide). Must be <= VIA_EXTRA_MACRO_STORED_CHARS
#define VIA_EXTRA_MACRO_LINE_CHARS    5  // characters per line on the key: a name longer than this wraps onto a second line

// Name the user gave macro 'macro' through the protocol, or NULL if it has none (or macro is out of range)
const char *via_extra_macro_name(uint8_t macro);

// One-time conversion of an EEPROM written by firmware with 4 layers (keymap, macros and settings block at their
// old addresses) to the 8-layer layout. Call from via_init_kb() (buildbox.c): it must run before QMK's via_init()
// checks its EEPROM magic, which may reset the keymap and macros. Does nothing on a board that is already converted
// or on a blank EEPROM.
void via_extra_storage_migrate(void);

// Initialise from EEPROM: call once at boot, after eeprom_custom_load() and before the display is drawn
void via_extra_init(void);

/*** Adding/removing layers (the customized VIA app) is staged: ADD_LAYER / REMOVE_LAYER change a RAM copy of every
	per-layer item (keymap, names, icons, colors) and nothing reaches EEPROM until SAVE; CANCEL throws the copy away.
	While a copy exists the host sees it (layer count, keymap commands, names, icons, colors) and the keyboard itself
	keeps using the saved layers.
***/
bool    via_extra_layer_edit_pending(void);
uint8_t via_extra_layer_count(void);             // layers the HOST sees: the staged count while editing, else layer_active_count()
// VIA keymap commands the layer count affects (id_dynamic_keymap_get_layer_count / get_/set_keycode / get_/set_buffer),
// answered from the staged copy while one exists. 'data' is rewritten into the reply. Returns true if handled
// (the caller sends the reply); false = leave it to VIA/QMK (also cancels a staged copy on a keymap reset).
bool    via_extra_keymap_command(uint8_t *data);
// Per-layer lighting color (VIA custom value id_rgb_layers_hue): staged copy while editing, else eepdata
void    via_extra_layer_color_get(uint8_t layer, uint8_t *hue, uint8_t *sat);
void    via_extra_layer_color_set(uint8_t layer, uint8_t hue, uint8_t sat);

// Name to show for 'layer': the custom name if one is set, otherwise the built-in layer_names[] entry
const char *via_extra_layer_name(uint8_t layer);

// Handle one raw HID report whose byte 0 is VIA_EXTRA_CMD. 'data' is rewritten in place into the reply
// (the caller sends it back). Returns true if the request changed what the LCD shows or what is
// pending in EEPROM, i.e. it counts as the user doing something (wakes the screen).
bool via_extra_handle(uint8_t *data, uint8_t length);
