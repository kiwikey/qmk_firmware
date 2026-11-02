#include "via_extra.h"

#include "buildbox.h"                 // KC_BUTTON_1/2: a new layer keeps the two buttons
#include "via.h"                      // id_dynamic_keymap_*: the keymap commands answered while layers are being edited
#include "eeprom_custom.h"
#include "display/defines.h"          // layer_names[]
#include "display/qp_graphics.h"      // ui_refresh(), ui_idle_screen_visible()
#include "display/widgets/qp_widget_layer.h" // LAYER_ICON_POOL_COUNT

#define VIA_EXTRA_LAYERS       DYNAMIC_KEYMAP_LAYER_COUNT
#define VIA_EXTRA_ICON_COUNT   LAYER_ICON_POOL_COUNT

/*** EEPROM layout of the via_extra region (VIA_EXTRA_EEPROM_ADDR / VIA_EXTRA_EEPROM_SIZE, config.h)
	[0]  'I'   magic
	[1]  'Z'   magic
	[2]  storage version (VIA_EXTRA_STORAGE_VERSION)
	[3]  checksum over every record byte
	[4..] VIA_EXTRA_STORED_LAYERS records of VIA_EXTRA_RECORD_SIZE bytes: name[16] (zero padded), len, reserved (0)
	An empty record (len = 0) means "no custom name" - the built-in layer_names[] entry is used.
	8 records are stored although BuildBox has 4 layers, so adding layers later doesn't move anything.
***/
#define VIA_EXTRA_STORED_LAYERS   8
#define VIA_EXTRA_RECORD_SIZE     (VIA_EXTRA_NAME_FIELD + 2)
#define VIA_EXTRA_HEADER_SIZE     4
#define VIA_EXTRA_IMAGE_SIZE      (VIA_EXTRA_HEADER_SIZE + VIA_EXTRA_STORED_LAYERS * VIA_EXTRA_RECORD_SIZE)
// Version 1 = written by firmware with 4 layers (old keymap/macro addresses) - via_extra_storage_migrate() upgrades it to 2.
#define VIA_EXTRA_STORAGE_VERSION 2
// The magic 'I' 'Z' is left from the protocol's old name (IZX) on purpose: it is stored data,
// changing it would make every board forget its saved layer names.
#define VIA_EXTRA_MAGIC0          'I'
#define VIA_EXTRA_MAGIC1          'Z'

_Static_assert(VIA_EXTRA_IMAGE_SIZE <= VIA_EXTRA_EEPROM_SIZE, "VIA_EXTRA_EEPROM_SIZE (config.h) is too small for the via_extra storage layout");
_Static_assert(VIA_EXTRA_LAYERS <= VIA_EXTRA_STORED_LAYERS, "more layers than via_extra stores names for - raise VIA_EXTRA_STORED_LAYERS");
_Static_assert(VIA_EXTRA_NAME_MAX <= VIA_EXTRA_NAME_FIELD, "VIA_EXTRA_NAME_MAX cannot exceed the 16-byte name field");
_Static_assert(DYNAMIC_KEYMAP_EEPROM_MAX_ADDR == VIA_EXTRA_MACRO_EEPROM_ADDR - 1, "DYNAMIC_KEYMAP_EEPROM_MAX_ADDR (config.h, a plain number) must be VIA_EXTRA_MACRO_EEPROM_ADDR - 1 - update it if a via_extra region size changes");
_Static_assert(VIA_EXTRA_EEPROM_ADDR + VIA_EXTRA_EEPROM_SIZE == TOTAL_EEPROM_BYTE_COUNT, "the via_extra region must end exactly at the end of EEPROM - check DYNAMIC_KEYMAP_EEPROM_MAX_ADDR (config.h)");

/*** EEPROM layout of the macro-name region (VIA_EXTRA_MACRO_EEPROM_ADDR / _SIZE, config.h)
	[0]  'M'   magic
	[1]  'N'   magic
	[2]  storage version (VIA_EXTRA_MACRO_STORAGE_VERSION)
	[3]  checksum over every record byte
	[4..] VIA_EXTRA_STORED_MACROS records of VIA_EXTRA_MACRO_RECORD_SIZE bytes: len, name[10] (zero padded)
	Names are short (they are drawn on a key, 2 lines of 5), so a record keeps up to 10 characters.
	Version 2: records grew from 9 to 11 bytes (8 -> 10 characters), so version 1 data is dropped at boot.
***/
#define VIA_EXTRA_STORED_MACROS         16
#define VIA_EXTRA_MACRO_STORED_CHARS    10
#define VIA_EXTRA_MACRO_RECORD_SIZE     (1 + VIA_EXTRA_MACRO_STORED_CHARS)
#define VIA_EXTRA_MACRO_IMAGE_SIZE      (VIA_EXTRA_HEADER_SIZE + VIA_EXTRA_STORED_MACROS * VIA_EXTRA_MACRO_RECORD_SIZE)
#define VIA_EXTRA_MACRO_STORAGE_VERSION 2
#define VIA_EXTRA_MACRO_MAGIC0          'M'
#define VIA_EXTRA_MACRO_MAGIC1          'N'

_Static_assert(VIA_EXTRA_MACRO_IMAGE_SIZE <= VIA_EXTRA_MACRO_EEPROM_SIZE, "VIA_EXTRA_MACRO_EEPROM_SIZE (config.h) is too small for the macro-name storage layout");
_Static_assert(VIA_EXTRA_MACRO_COUNT <= VIA_EXTRA_STORED_MACROS, "more macros than via_extra stores names for - raise VIA_EXTRA_STORED_MACROS");
_Static_assert(VIA_EXTRA_MACRO_NAME_MAX <= VIA_EXTRA_MACRO_STORED_CHARS, "a macro record cannot hold VIA_EXTRA_MACRO_NAME_MAX characters - raise VIA_EXTRA_MACRO_STORED_CHARS");
_Static_assert(VIA_EXTRA_MACRO_NAME_MAX <= VIA_EXTRA_NAME_FIELD, "VIA_EXTRA_MACRO_NAME_MAX cannot exceed the 16-byte name field");
_Static_assert(VIA_EXTRA_MACRO_NAME_MAX <= 2 * VIA_EXTRA_MACRO_LINE_CHARS, "a macro name is drawn on at most 2 lines of VIA_EXTRA_MACRO_LINE_CHARS characters");
_Static_assert(VIA_EXTRA_MACRO_EEPROM_ADDR + VIA_EXTRA_MACRO_EEPROM_SIZE == VIA_EXTRA_EEPROM_ADDR, "the macro-name region must end exactly where the layer-name region starts");

// RAM copy of the custom macro names, same idea as 'custom' below.
static struct {
	uint8_t len;
	char    name[VIA_EXTRA_MACRO_STORED_CHARS + 1]; // always NUL-terminated
} macro_custom[VIA_EXTRA_MACRO_COUNT];
static bool macro_names_dirty = false;

// RAM copy of the custom names. A pending change lives only here until SAVE.
typedef struct {
	uint8_t len;
	char    name[VIA_EXTRA_NAME_FIELD + 1]; // always NUL-terminated
} layer_name_t;
static layer_name_t custom[VIA_EXTRA_LAYERS];
static bool names_dirty = false; // changed since the last load/save

/*** Layers being added/removed: a RAM copy of everything that belongs to a layer (see the comment in via_extra.h).
	'active' = a copy exists. It is made by the first ADD_LAYER/REMOVE_LAYER, written out by SAVE (layer_edit_commit())
	and thrown away by CANCEL. Names, icons and colors of the layers are in the copy too, because they move with their layer.
***/
#define VIA_EXTRA_KEYMAP_BYTES (MATRIX_ROWS * MATRIX_COLS * 2) // one layer of the VIA keymap buffer: 16-bit keycodes, row by row
_Static_assert(VIA_EXTRA_LAYERS == EEPROM_CUSTOM_MAX_LAYERS, "the per-layer arrays in eepdata must have one entry per layer");
static struct {
	bool         active;
	uint8_t      count;
	uint8_t      keymap[VIA_EXTRA_LAYERS * VIA_EXTRA_KEYMAP_BYTES];
	layer_name_t name[VIA_EXTRA_LAYERS];
	uint8_t      icon[VIA_EXTRA_LAYERS];
	uint8_t      hue[VIA_EXTRA_LAYERS];
	uint8_t      sat[VIA_EXTRA_LAYERS];
} edit;

/*** Redraw the idle screen after a SET (names/icons show on it). Debounced like the theme-color
	repaint in via_custom.c: a host that sends several SETs in a row repaints once.
***/
#define VIA_EXTRA_REPAINT_DEBOUNCE_MS 150
static deferred_token repaint_token = INVALID_DEFERRED_TOKEN;

static uint32_t via_extra_repaint_callback(uint32_t trigger_time, void *cb_arg) {
	repaint_token = INVALID_DEFERRED_TOKEN;
	// Whatever covers the idle screen (menu, Breakout, tutorial, screensaver) calls
	// ui_refresh() itself when it closes, so it picks the new names up then.
	if (ui_idle_screen_visible()) {
		ui_refresh();
	}
	return 0; // one-shot, don't requeue
}

static void via_extra_request_repaint(void) {
	if (repaint_token == INVALID_DEFERRED_TOKEN ||
		!extend_deferred_exec(repaint_token, VIA_EXTRA_REPAINT_DEBOUNCE_MS)) {
		repaint_token = defer_exec(VIA_EXTRA_REPAINT_DEBOUNCE_MS, via_extra_repaint_callback, NULL);
	}
}

/*** Storage ***/
static uint8_t via_extra_checksum_n(const uint8_t *bytes, uint16_t count) {
	uint8_t sum = 0xA5;
	for (uint16_t i = 0; i < count; i++) {
		sum = (uint8_t)((sum << 1) | (sum >> 7)) ^ bytes[i];
	}
	return sum;
}

static uint8_t via_extra_checksum(const uint8_t *records) {
	return via_extra_checksum_n(records, VIA_EXTRA_STORED_LAYERS * VIA_EXTRA_RECORD_SIZE); // layer-name records
}

static void via_extra_build_image(uint8_t *image) {
	memset(image, 0, VIA_EXTRA_IMAGE_SIZE);
	uint8_t *records = image + VIA_EXTRA_HEADER_SIZE;
	for (uint8_t i = 0; i < VIA_EXTRA_LAYERS; i++) {
		uint8_t *record = records + i * VIA_EXTRA_RECORD_SIZE;
		memcpy(record, custom[i].name, custom[i].len);
		record[VIA_EXTRA_NAME_FIELD] = custom[i].len;
	}
	image[0] = VIA_EXTRA_MAGIC0;
	image[1] = VIA_EXTRA_MAGIC1;
	image[2] = VIA_EXTRA_STORAGE_VERSION;
	image[3] = via_extra_checksum(records);
}

// Write the names to EEPROM (only changed bytes are actually written) and read them back to make sure
static bool via_extra_store(void) {
	uint8_t image[VIA_EXTRA_IMAGE_SIZE];
	uint8_t check[VIA_EXTRA_IMAGE_SIZE];
	via_extra_build_image(image);
	eeprom_update_block(image, (void *)VIA_EXTRA_EEPROM_ADDR, VIA_EXTRA_IMAGE_SIZE);
	eeprom_read_block(check, (const void *)VIA_EXTRA_EEPROM_ADDR, VIA_EXTRA_IMAGE_SIZE);
	return memcmp(image, check, VIA_EXTRA_IMAGE_SIZE) == 0;
}

/*** Macro-name storage - the same scheme as the layer names above ***/
static void via_extra_macro_build_image(uint8_t *image) {
	memset(image, 0, VIA_EXTRA_MACRO_IMAGE_SIZE);
	uint8_t *records = image + VIA_EXTRA_HEADER_SIZE;
	for (uint8_t i = 0; i < VIA_EXTRA_MACRO_COUNT; i++) {
		uint8_t *record = records + i * VIA_EXTRA_MACRO_RECORD_SIZE;
		record[0] = macro_custom[i].len;
		memcpy(record + 1, macro_custom[i].name, macro_custom[i].len);
	}
	image[0] = VIA_EXTRA_MACRO_MAGIC0;
	image[1] = VIA_EXTRA_MACRO_MAGIC1;
	image[2] = VIA_EXTRA_MACRO_STORAGE_VERSION;
	image[3] = via_extra_checksum_n(records, VIA_EXTRA_STORED_MACROS * VIA_EXTRA_MACRO_RECORD_SIZE);
}

static bool via_extra_macro_store(void) {
	uint8_t image[VIA_EXTRA_MACRO_IMAGE_SIZE];
	uint8_t check[VIA_EXTRA_MACRO_IMAGE_SIZE];
	via_extra_macro_build_image(image);
	eeprom_update_block(image, (void *)VIA_EXTRA_MACRO_EEPROM_ADDR, VIA_EXTRA_MACRO_IMAGE_SIZE);
	eeprom_read_block(check, (const void *)VIA_EXTRA_MACRO_EEPROM_ADDR, VIA_EXTRA_MACRO_IMAGE_SIZE);
	return memcmp(image, check, VIA_EXTRA_MACRO_IMAGE_SIZE) == 0;
}

static void via_extra_macro_load(void) {
	uint8_t image[VIA_EXTRA_MACRO_IMAGE_SIZE];
	eeprom_read_block(image, (const void *)VIA_EXTRA_MACRO_EEPROM_ADDR, VIA_EXTRA_MACRO_IMAGE_SIZE);

	memset(macro_custom, 0, sizeof(macro_custom));
	macro_names_dirty = false;

	bool valid = image[0] == VIA_EXTRA_MACRO_MAGIC0 && image[1] == VIA_EXTRA_MACRO_MAGIC1 && image[2] == VIA_EXTRA_MACRO_STORAGE_VERSION
	          && image[3] == via_extra_checksum_n(image + VIA_EXTRA_HEADER_SIZE, VIA_EXTRA_STORED_MACROS * VIA_EXTRA_MACRO_RECORD_SIZE);
	if (!valid) {
		via_extra_macro_store(); // fresh or erased EEPROM (or the region held old macro bytes): no names
		return;
	}

	for (uint8_t i = 0; i < VIA_EXTRA_MACRO_COUNT; i++) {
		const uint8_t *record = image + VIA_EXTRA_HEADER_SIZE + i * VIA_EXTRA_MACRO_RECORD_SIZE;
		uint8_t        len    = record[0];
		bool           ok     = len <= VIA_EXTRA_MACRO_NAME_MAX; // a name longer than today's limit is not shown
		for (uint8_t j = 0; ok && j < len; j++) {
			ok = record[1 + j] >= 0x20 && record[1 + j] <= 0x7E;
		}
		if (ok && len > 0) {
			macro_custom[i].len = len;
			memcpy(macro_custom[i].name, record + 1, len); // [len] stays 0 from the memset
		}
	}
}

const char *via_extra_macro_name(uint8_t macro) {
	if (macro >= VIA_EXTRA_MACRO_COUNT || macro_custom[macro].len == 0) return NULL;
	return macro_custom[macro].name;
}

// Layer names from EEPROM into 'custom' (a blank/invalid region = no custom names, and is initialised)
static void via_extra_names_load(void) {
	uint8_t image[VIA_EXTRA_IMAGE_SIZE];
	eeprom_read_block(image, (const void *)VIA_EXTRA_EEPROM_ADDR, VIA_EXTRA_IMAGE_SIZE);

	memset(custom, 0, sizeof(custom));
	names_dirty = false;

	bool valid = image[0] == VIA_EXTRA_MAGIC0 && image[1] == VIA_EXTRA_MAGIC1 && image[2] == VIA_EXTRA_STORAGE_VERSION
	          && image[3] == via_extra_checksum(image + VIA_EXTRA_HEADER_SIZE);
	if (!valid) {
		// Fresh or erased EEPROM (or the region held old macro bytes): start with no custom names
		via_extra_store();
		return;
	}

	for (uint8_t i = 0; i < VIA_EXTRA_LAYERS; i++) {
		const uint8_t *record = image + VIA_EXTRA_HEADER_SIZE + i * VIA_EXTRA_RECORD_SIZE;
		uint8_t        len    = record[VIA_EXTRA_NAME_FIELD];
		bool           ok     = len <= VIA_EXTRA_NAME_MAX;
		for (uint8_t j = 0; ok && j < len; j++) {
			ok = record[j] >= 0x20 && record[j] <= 0x7E;
		}
		if (ok && len > 0) {
			custom[i].len = len;
			memcpy(custom[i].name, record, len); // [len] stays 0 from the memset
		}
	}
}

// The two buttons below the screen (last matrix row, columns 0 and 1) as 16-bit keycodes at 'at' (the start of a layer in
// the VIA keymap buffer). They change layers, so every layer has them - buildbox.c also treats a KC_NO there as the button.
static void layer_set_buttons(uint8_t *at) {
	for (uint8_t col = 0; col < 2; col++) {
		uint16_t keycode = KC_BUTTON_1 + col;
		at[((MATRIX_ROWS - 1) * MATRIX_COLS + col) * 2]     = keycode >> 8;
		at[((MATRIX_ROWS - 1) * MATRIX_COLS + col) * 2 + 1] = keycode & 0xFF;
	}
}

/*** One-time conversion from the 4-layer EEPROM layout (see via_extra_storage_migrate() in via_extra.h)
	old: [settings block 26 B][keymap 4 layers][macros ...]     new: [settings block 39 B][keymap 8 layers][macros ...]
	The layer-name region (end of EEPROM) did not move, and says which layout the keymap is in: its version 1 = old.
***/
#define VIA_EXTRA_OLD_LAYERS          4
#define VIA_EXTRA_OLD_SETTINGS_SIZE   26
#define VIA_EXTRA_OLD_SETTINGS_VERSION 8
// Storage version while the keymap/macros are being moved: a power cut leaves it there, and the next boot knows
// the move was cut off (it overwrote part of what it copies from, so it can't simply run again).
#define VIA_EXTRA_STORAGE_MIGRATING   0x81

typedef struct { // the settings block as it was in layout version 8
	uint8_t active_layer;
	uint8_t display_bootanim;
	uint8_t display_timeout;
	uint8_t display_brightness;
	uint8_t lighting_layers;
	uint8_t layer_hue[VIA_EXTRA_OLD_LAYERS];
	uint8_t layer_sat[VIA_EXTRA_OLD_LAYERS];
	uint8_t knob_effect;
	uint8_t knob_func;
	uint8_t theme_hue;
	uint8_t knob_sensitivity;
	uint8_t unbox_tutorial;
	uint8_t screensaver_effect;
	uint8_t layer_icon[VIA_EXTRA_OLD_LAYERS];
	uint8_t screensaver_idle_min;
	uint8_t dial_hold_100ms;
	uint8_t layout_version;
} eeprom_custom_v8_t;
_Static_assert(sizeof(eeprom_custom_v8_t) == VIA_EXTRA_OLD_SETTINGS_SIZE, "eeprom_custom_v8_t must be the 26-byte layout-8 settings block");

void via_extra_storage_migrate(void) {
	uint8_t header[4];
	eeprom_read_block(header, (const void *)VIA_EXTRA_EEPROM_ADDR, sizeof(header));
	if (header[0] != VIA_EXTRA_MAGIC0 || header[1] != VIA_EXTRA_MAGIC1) return; // blank/erased EEPROM
	if (header[2] != 1 && header[2] != VIA_EXTRA_STORAGE_MIGRATING) return;     // already converted

	// QMK's via_init() runs right after this (via_init_kb() is its first step). Its EEPROM magic is the firmware's
	// build date, so firmware built on another day than the one in EEPROM makes it reset the keymap and macros
	// anyway, at the new addresses. So the old ones are moved only when VIA will keep them: same build date, and no
	// earlier move that was cut off. Otherwise VIA is told to reset them, and only the settings are converted.
	bool move_keymap = header[2] == 1 && via_eeprom_is_valid();

	const uint32_t settings = VIA_EEPROM_CUSTOM_CONFIG_ADDR;
	const uint32_t keymap_old = settings + VIA_EXTRA_OLD_SETTINGS_SIZE;
	const uint32_t macro_old  = keymap_old + VIA_EXTRA_OLD_LAYERS * VIA_EXTRA_KEYMAP_BYTES;
	const uint32_t keymap_new = settings + sizeof(eeprom_custom_t);
	const uint32_t macro_new  = keymap_new + VIA_EXTRA_LAYERS * VIA_EXTRA_KEYMAP_BYTES;
	const uint32_t macro_len  = (uint32_t)DYNAMIC_KEYMAP_EEPROM_MAX_ADDR + 1 - macro_new; // the new, smaller macro buffer; the end of the old one is dropped

	// The moves below only write from keymap_new up, so the old settings (bytes 0-25) are still intact after a cut-off move
	eeprom_custom_v8_t old;
	eeprom_read_block(&old, (const void *)(uintptr_t)settings, sizeof(old));

	if (move_keymap) {
		eeprom_update_byte((void *)(uintptr_t)(VIA_EXTRA_EEPROM_ADDR + 2), VIA_EXTRA_STORAGE_MIGRATING);

		// Everything moves UP, so each copy goes from the end downwards (the old and new areas overlap).
		// 1. macros first: their new place overlaps the old keymap/macro area, the keymap must still be intact
		for (uint32_t i = macro_len; i > 0; i--) {
			eeprom_update_byte((void *)(uintptr_t)(macro_new + i - 1), eeprom_read_byte((const uint8_t *)(uintptr_t)(macro_old + i - 1)));
		}
		// 2. the 4 existing layers
		for (uint32_t i = VIA_EXTRA_OLD_LAYERS * VIA_EXTRA_KEYMAP_BYTES; i > 0; i--) {
			eeprom_update_byte((void *)(uintptr_t)(keymap_new + i - 1), eeprom_read_byte((const uint8_t *)(uintptr_t)(keymap_old + i - 1)));
		}
		// 3. layers 4..7 start blank (KC_NO, but with the 2 buttons) - that area held the start of the old macros
		for (uint8_t layer = VIA_EXTRA_OLD_LAYERS; layer < VIA_EXTRA_LAYERS; layer++) {
			uint8_t blank[VIA_EXTRA_KEYMAP_BYTES] = { 0 };
			layer_set_buttons(blank);
			for (uint32_t i = 0; i < VIA_EXTRA_KEYMAP_BYTES; i++) {
				eeprom_update_byte((void *)(uintptr_t)(keymap_new + layer * VIA_EXTRA_KEYMAP_BYTES + i), blank[i]);
			}
		}
	} else {
		// Nothing usable at the new addresses: via_init() resets the keymap (from keymaps[]) and the macros right after this
		via_eeprom_set_valid(false);
	}

	// 4. the settings block: layout 8 settings carried over, the 4 new layers and the layer count at their defaults.
	// (A power cut during or right after this step leaves old.layout_version unrecognised: the settings go back to defaults.)
	eeprom_custom_t block = eepdata_default;
	if (old.layout_version == VIA_EXTRA_OLD_SETTINGS_VERSION) {
		block.active_layer        = old.active_layer;
		block.display_bootanim    = old.display_bootanim;
		block.display_timeout     = old.display_timeout;
		block.display_brightness  = old.display_brightness;
		block.lighting_layers     = old.lighting_layers;
		memcpy(block.layer_hue,  old.layer_hue,  VIA_EXTRA_OLD_LAYERS);
		memcpy(block.layer_sat,  old.layer_sat,  VIA_EXTRA_OLD_LAYERS);
		block.knob_effect         = old.knob_effect;
		block.knob_func           = old.knob_func;
		block.theme_hue           = old.theme_hue;
		block.knob_sensitivity    = old.knob_sensitivity;
		block.unbox_tutorial      = old.unbox_tutorial;
		block.screensaver_effect  = old.screensaver_effect;
		memcpy(block.layer_icon, old.layer_icon, VIA_EXTRA_OLD_LAYERS);
		block.screensaver_idle_min = old.screensaver_idle_min;
		block.dial_hold_100ms     = old.dial_hold_100ms;
	}
	eeprom_update_block(&block, (void *)(uintptr_t)settings, sizeof(block));

	// 5. last: mark the conversion done (a power cut before this: the next boot sees VIA_EXTRA_STORAGE_MIGRATING, or still 1
	//    if nothing was moved yet, and converts again without moving anything twice)
	eeprom_update_byte((void *)(uintptr_t)(VIA_EXTRA_EEPROM_ADDR + 2), VIA_EXTRA_STORAGE_VERSION);
}

void via_extra_init(void) {
	via_extra_macro_load();
	via_extra_names_load();
	edit.active = false;
}

/*** Staged layer add/remove (see the comment in via_extra.h) ***/
bool    via_extra_layer_edit_pending(void) { return edit.active; }
uint8_t via_extra_layer_count(void)        { return edit.active ? edit.count : layer_active_count(); }

// A layer slot as a brand-new layer: blank keymap (KC_NO) except the 2 buttons, no custom name, default icon and color
static void layer_edit_reset_slot(uint8_t layer) {
	memset(&edit.keymap[layer * VIA_EXTRA_KEYMAP_BYTES], 0, VIA_EXTRA_KEYMAP_BYTES);
	layer_set_buttons(&edit.keymap[layer * VIA_EXTRA_KEYMAP_BYTES]);
	memset(&edit.name[layer], 0, sizeof(edit.name[layer]));
	edit.icon[layer] = eepdata_default.layer_icon[layer];
	edit.hue[layer]  = eepdata_default.layer_hue[layer];
	edit.sat[layer]  = eepdata_default.layer_sat[layer];
}

// Start the staged copy from what the keyboard has now (including names/icons/colors not saved yet)
static void layer_edit_begin(void) {
	if (edit.active) return;
	dynamic_keymap_get_buffer(0, sizeof(edit.keymap), edit.keymap);
	memcpy(edit.name, custom, sizeof(edit.name));
	memcpy(edit.icon, eepdata.layer_icon, sizeof(edit.icon));
	memcpy(edit.hue,  eepdata.layer_hue,  sizeof(edit.hue));
	memcpy(edit.sat,  eepdata.layer_sat,  sizeof(edit.sat));
	edit.count  = layer_active_count();
	edit.active = true;
}

// false = refused (already at the maximum)
static bool layer_edit_add(void) {
	if (via_extra_layer_count() >= VIA_EXTRA_LAYERS) return false;
	layer_edit_begin();
	layer_edit_reset_slot(edit.count);
	edit.count++;
	return true;
}

// false = refused: layer 0 (the default layer) stays, the layer must exist, and one layer must be left
static bool layer_edit_remove(uint8_t index) {
	if (index == 0 || index >= via_extra_layer_count() || via_extra_layer_count() <= LAYER_COUNT_MIN) return false;
	layer_edit_begin();
	// A built-in name belongs to a position ("Media" is layer 1), so a layer that moves keeps the name it showed
	for (uint8_t i = index + 1; i < edit.count; i++) {
		if (edit.name[i].len == 0) {
			uint8_t len = (uint8_t)strnlen(layer_names[i], VIA_EXTRA_NAME_MAX);
			edit.name[i].len = len;
			memcpy(edit.name[i].name, layer_names[i], len); // [len] is already 0
		}
	}
	uint8_t last = edit.count - 1;
	for (uint8_t i = index; i < last; i++) {
		memcpy(&edit.keymap[i * VIA_EXTRA_KEYMAP_BYTES], &edit.keymap[(i + 1) * VIA_EXTRA_KEYMAP_BYTES], VIA_EXTRA_KEYMAP_BYTES);
		edit.name[i] = edit.name[i + 1];
		edit.icon[i] = edit.icon[i + 1];
		edit.hue[i]  = edit.hue[i + 1];
		edit.sat[i]  = edit.sat[i + 1];
	}
	layer_edit_reset_slot(last); // the removed layer's data is gone
	edit.count--;
	return true;
}

// SAVE: write the staged layers to EEPROM. false = the keymap did not read back as written (the staged copy is kept)
static bool layer_edit_commit(void) {
	if (!edit.active) return true;

	dynamic_keymap_set_buffer(0, sizeof(edit.keymap), edit.keymap);
	for (uint16_t offset = 0; offset < sizeof(edit.keymap); offset += 32) { // read back, in small pieces (stack)
		uint8_t piece[32];
		dynamic_keymap_get_buffer(offset, sizeof(piece), piece);
		if (memcmp(piece, &edit.keymap[offset], sizeof(piece)) != 0) return false;
	}

	eepdata.layer_count = edit.count;
	memcpy(eepdata.layer_icon, edit.icon, sizeof(edit.icon));
	memcpy(eepdata.layer_hue,  edit.hue,  sizeof(edit.hue));
	memcpy(eepdata.layer_sat,  edit.sat,  sizeof(edit.sat));
	memcpy(custom, edit.name, sizeof(custom));
	names_dirty = true; // SAVE stores the names right after this
	edit.active = false;

	// Layers that no longer exist cannot stay active
	if (eepdata.active_layer >= eepdata.layer_count) eepdata.active_layer = 0;
	layer_and(((layer_state_t)1 << eepdata.layer_count) - 1);
	via_extra_request_repaint();
	return true;
}

// CANCEL: back to what is in EEPROM - the staged layers go, and so do names not saved yet
static void layer_edit_cancel(void) {
	edit.active = false;
	via_extra_macro_load(); // reload from EEPROM (resets the unsaved flags too)
	via_extra_names_load();
	via_extra_request_repaint();
}

bool via_extra_keymap_command(uint8_t *data) {
	uint8_t *cd = &data[1]; // the command's data, like VIA's own command_data
	switch (data[0]) {
		case id_dynamic_keymap_get_layer_count:
			cd[0] = via_extra_layer_count();
			return true;
		case id_dynamic_keymap_reset:
			edit.active = false; // QMK rewrites the saved keymap: a staged copy of the old one is meaningless
			return false;
		case id_dynamic_keymap_get_keycode: {
			if (!edit.active) return false;
			uint16_t keycode = KC_NO;
			if (cd[0] < VIA_EXTRA_LAYERS && cd[1] < MATRIX_ROWS && cd[2] < MATRIX_COLS) {
				const uint8_t *at = &edit.keymap[cd[0] * VIA_EXTRA_KEYMAP_BYTES + (cd[1] * MATRIX_COLS + cd[2]) * 2];
				keycode = (uint16_t)((at[0] << 8) | at[1]);
			}
			cd[3] = keycode >> 8;
			cd[4] = keycode & 0xFF;
			return true;
		}
		case id_dynamic_keymap_set_keycode: {
			if (!edit.active) return false;
			if (cd[0] < VIA_EXTRA_LAYERS && cd[1] < MATRIX_ROWS && cd[2] < MATRIX_COLS) {
				uint8_t *at = &edit.keymap[cd[0] * VIA_EXTRA_KEYMAP_BYTES + (cd[1] * MATRIX_COLS + cd[2]) * 2];
				at[0] = cd[3];
				at[1] = cd[4];
			}
			return true;
		}
		case id_dynamic_keymap_get_buffer: {
			if (!edit.active) return false;
			uint16_t offset = (cd[0] << 8) | cd[1];
			uint16_t size   = cd[2]; // <= 28
			for (uint16_t i = 0; i < size; i++) {
				cd[3 + i] = (offset + i < sizeof(edit.keymap)) ? edit.keymap[offset + i] : 0;
			}
			return true;
		}
		case id_dynamic_keymap_set_buffer: {
			if (!edit.active) return false;
			uint16_t offset = (cd[0] << 8) | cd[1];
			uint16_t size   = cd[2]; // <= 28
			for (uint16_t i = 0; i < size; i++) {
				if (offset + i < sizeof(edit.keymap)) edit.keymap[offset + i] = cd[3 + i];
			}
			return true;
		}
		default:
			return false;
	}
}

void via_extra_layer_color_get(uint8_t layer, uint8_t *hue, uint8_t *sat) {
	if (layer >= VIA_EXTRA_LAYERS) { *hue = 0; *sat = 0; return; }
	*hue = edit.active ? edit.hue[layer] : eepdata.layer_hue[layer];
	*sat = edit.active ? edit.sat[layer] : eepdata.layer_sat[layer];
}

void via_extra_layer_color_set(uint8_t layer, uint8_t hue, uint8_t sat) {
	if (layer >= VIA_EXTRA_LAYERS) return;
	if (edit.active) {
		edit.hue[layer] = hue;
		edit.sat[layer] = sat;
	} else {
		eepdata.layer_hue[layer] = hue;
		eepdata.layer_sat[layer] = sat;
	}
}

const char *via_extra_layer_name(uint8_t layer) {
	layer %= VIA_EXTRA_LAYERS; // layer_names[] has exactly VIA_EXTRA_LAYERS entries
	return custom[layer].len ? custom[layer].name : layer_names[layer];
}

/*** The protocol ***/

// Argument length of every operation (spec section 4), or -1 for an unknown one
static int8_t via_extra_args_len(uint8_t op) {
	switch (op) {
		case VIA_EXTRA_OP_GET_CAPS:       return 0;
		case VIA_EXTRA_OP_GET_LAYER_NAME: return 1;
		case VIA_EXTRA_OP_SET_LAYER_NAME: return 1 + 1 + VIA_EXTRA_NAME_FIELD; // layer, len, name field
		case VIA_EXTRA_OP_GET_LAYER_ICON: return 1;
		case VIA_EXTRA_OP_SET_LAYER_ICON: return 2;
		case VIA_EXTRA_OP_SAVE:           return 0;
		case VIA_EXTRA_OP_CANCEL:         return 0;
		case VIA_EXTRA_OP_GET_LAYER_COUNT: return 0;
		case VIA_EXTRA_OP_ADD_LAYER:      return 0;
		case VIA_EXTRA_OP_REMOVE_LAYER:   return 1; // layer index
		case VIA_EXTRA_OP_GET_STATE:      return 0;
		case VIA_EXTRA_OP_GET_MACRO_NAME: return 1;
		case VIA_EXTRA_OP_SET_MACRO_NAME: return 1 + 1 + VIA_EXTRA_NAME_FIELD; // macro, len, name field
		case VIA_EXTRA_OP_GET_SETTING:    return 1;     // id
		case VIA_EXTRA_OP_SET_SETTING:    return 1 + 2; // id, hi, lo
		default:                    return -1;
	}
}

bool via_extra_handle(uint8_t *data, uint8_t length) {
	// The reply is this same buffer: bytes 0 .. 2+ARGS_LEN are the request and stay untouched,
	// STATUS goes at 3+ARGS_LEN and the data after it (spec section 2).
	uint8_t version = data[1];
	uint8_t op      = data[2];
	int8_t  args    = via_extra_args_len(op);
	bool    changed = false;

	if (args < 0) {
		data[3] = VIA_EXTRA_STATUS_UNKNOWN_OP; // unknown op: its ARGS length is unknown, so STATUS goes right after OP
	} else {
		uint8_t *status = &data[3 + args];
		uint8_t *out    = status + 1; // first data byte
		uint8_t  used   = 0;          // data bytes written at 'out'
		*status         = VIA_EXTRA_STATUS_OK;

		if (version > VIA_EXTRA_VERSION && op != VIA_EXTRA_OP_GET_CAPS) { // GET_CAPS is always allowed, whatever VER says
			*status = VIA_EXTRA_STATUS_UNSUPPORTED_VERSION;
		} else {
			switch (op) {
				case VIA_EXTRA_OP_GET_CAPS: {
					out[0] = VIA_EXTRA_VERSION;
					out[1] = VIA_EXTRA_FEATURE_NAMES | VIA_EXTRA_FEATURE_ICONS | VIA_EXTRA_FEATURE_GET_STATE | VIA_EXTRA_FEATURE_SETTINGS | VIA_EXTRA_FEATURE_MACRO_NAMES | VIA_EXTRA_FEATURE_LAYER_EDIT; // 0x77
					out[2] = via_extra_layer_count();  // layers now (what the host sees, so it includes an unsaved add/remove)
					out[3] = VIA_EXTRA_NAME_MAX;
					out[4] = VIA_EXTRA_ICON_COUNT;
					out[5] = VIA_EXTRA_MACRO_COUNT;    // byte 9: only meaningful with features bit 5
					out[6] = VIA_EXTRA_MACRO_NAME_MAX; // byte 10
					out[7] = VIA_EXTRA_LAYERS;         // byte 11: the most layers there can be (features bit 6)
					used   = 8;
					break;
				}
				case VIA_EXTRA_OP_GET_LAYER_COUNT: {
					out[0] = via_extra_layer_count();
					out[1] = VIA_EXTRA_LAYERS;
					out[2] = edit.active ? 1 : 0; // 1 = an add/remove is staged and not saved yet
					used   = 3;
					break;
				}
				case VIA_EXTRA_OP_ADD_LAYER: {
					if (!layer_edit_add()) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; } // already at the maximum
					out[0]  = via_extra_layer_count();
					used    = 1;
					changed = true;
					break;
				}
				case VIA_EXTRA_OP_REMOVE_LAYER: {
					if (!layer_edit_remove(data[3])) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; } // layer 0, a missing layer, or the last one left
					out[0]  = via_extra_layer_count();
					used    = 1;
					changed = true;
					break;
				}
				case VIA_EXTRA_OP_CANCEL: {
					layer_edit_cancel();
					changed = true;
					break;
				}
				case VIA_EXTRA_OP_GET_LAYER_NAME: {
					uint8_t layer = data[3];
					if (layer >= via_extra_layer_count()) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
					const char *name = via_extra_layer_name(layer);
					if (edit.active) { // the staged layers: their own custom name, else the built-in one
						name = edit.name[layer].len ? edit.name[layer].name : layer_names[layer];
					}
					uint8_t     len  = (uint8_t)strnlen(name, VIA_EXTRA_NAME_FIELD);
					out[0] = len;
					memset(out + 1, 0, VIA_EXTRA_NAME_FIELD);
					memcpy(out + 1, name, len);
					used = 1 + VIA_EXTRA_NAME_FIELD;
					break;
				}
				case VIA_EXTRA_OP_SET_LAYER_NAME: {
					uint8_t        layer = data[3];
					uint8_t        len   = data[4];
					const uint8_t *name  = &data[5];
					bool           ok    = layer < via_extra_layer_count() && len <= VIA_EXTRA_NAME_MAX;
					for (uint8_t i = 0; ok && i < len; i++) { // only the first 'len' bytes count; the rest is padding
						ok = name[i] >= 0x20 && name[i] <= 0x7E;
					}
					if (!ok) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
					layer_name_t *target = edit.active ? &edit.name[layer] : &custom[layer]; // staged layers have their own names
					if (target->len != len || memcmp(target->name, name, len) != 0) {
						memset(target, 0, sizeof(*target));
						target->len = len; // len = 0 clears the custom name: the built-in one is shown again
						memcpy(target->name, name, len);
						changed = true;
						if (!edit.active) { // the keyboard itself still uses the saved layers while some are staged
							names_dirty = true;
							via_extra_request_repaint();
						}
					}
					break;
				}
				case VIA_EXTRA_OP_GET_MACRO_NAME: {
					uint8_t macro = data[3];
					if (macro >= VIA_EXTRA_MACRO_COUNT) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
					uint8_t len = macro_custom[macro].len; // no built-in names for macros: len 0 = no name
					out[0] = len;
					memset(out + 1, 0, VIA_EXTRA_NAME_FIELD);
					memcpy(out + 1, macro_custom[macro].name, len);
					used = 1 + VIA_EXTRA_NAME_FIELD;
					break;
				}
				case VIA_EXTRA_OP_SET_MACRO_NAME: {
					uint8_t        macro = data[3];
					uint8_t        len   = data[4];
					const uint8_t *name  = &data[5];
					bool           ok    = macro < VIA_EXTRA_MACRO_COUNT && len <= VIA_EXTRA_MACRO_NAME_MAX;
					for (uint8_t i = 0; ok && i < len; i++) { // only the first 'len' bytes count; the rest is padding
						ok = name[i] >= 0x20 && name[i] <= 0x7E;
					}
					if (!ok) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
					if (macro_custom[macro].len != len || memcmp(macro_custom[macro].name, name, len) != 0) {
						memset(&macro_custom[macro], 0, sizeof(macro_custom[macro]));
						macro_custom[macro].len = len; // len = 0 clears the name: the key shows "Mn" again
						memcpy(macro_custom[macro].name, name, len);
						macro_names_dirty = true;
						changed           = true;
						via_extra_request_repaint();
					}
					break;
				}
				case VIA_EXTRA_OP_GET_LAYER_ICON: {
					uint8_t layer = data[3];
					if (layer >= via_extra_layer_count()) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
					out[0] = (uint8_t)((edit.active ? edit.icon[layer] : eepdata.layer_icon[layer]) + 1); // protocol id = pool index + 1, so never 0
					used   = 1;
					break;
				}
				case VIA_EXTRA_OP_SET_LAYER_ICON: {
					uint8_t layer = data[3];
					uint8_t icon  = data[4];
					if (layer >= via_extra_layer_count() || icon > VIA_EXTRA_ICON_COUNT) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
					uint8_t pool_index = icon ? (uint8_t)(icon - 1) : eepdata_default.layer_icon[layer]; // 0 = restore the layer's default icon
					uint8_t *target    = edit.active ? &edit.icon[layer] : &eepdata.layer_icon[layer];
					if (*target != pool_index) {
						*target = pool_index; // reaches EEPROM on SAVE, or whenever eeprom_custom_save() runs (spec section 4)
						changed = true;
						if (!edit.active) via_extra_request_repaint();
					}
					break;
				}
				case VIA_EXTRA_OP_SAVE: {
					bool ok = layer_edit_commit(); // staged add/remove first: it replaces the names and icons saved below
					if (names_dirty) {
						if (via_extra_store()) names_dirty = false; else ok = false;
					}
					if (macro_names_dirty) {
						if (via_extra_macro_store()) macro_names_dirty = false; else ok = false;
					}
					eeprom_custom_save(); // icons
					if (!ok) *status = VIA_EXTRA_STATUS_STORAGE_ERROR;
					break;
				}
				case VIA_EXTRA_OP_GET_STATE: {
					uint32_t mask = (uint32_t)(layer_state | default_layer_state) & ((1UL << VIA_EXTRA_LAYERS) - 1); // layer 0 is always on (default layer)
					out[0] = get_highest_layer(layer_state | default_layer_state);
					out[1] = (uint8_t)(mask);
					out[2] = (uint8_t)(mask >> 8);
					out[3] = (uint8_t)(mask >> 16);
					out[4] = (uint8_t)(mask >> 24);
					used   = 5;
					break;
				}
				case VIA_EXTRA_OP_GET_SETTING: {
					uint16_t value = 0;
					switch (data[3]) {
						case VIA_EXTRA_SETTING_SCREENSAVER_IDLE_MINUTES: value = eepdata.screensaver_idle_min; break;
						case VIA_EXTRA_SETTING_DIAL_SETTING_HOLD_MS:     value = (uint16_t)dial_settings_hold_ms(); break;
						default: *status = VIA_EXTRA_STATUS_BAD_ARGS; break;
					}
					if (*status != VIA_EXTRA_STATUS_OK) break;
					out[0] = (uint8_t)(value >> 8);
					out[1] = (uint8_t)value;
					used   = 2;
					break;
				}
				case VIA_EXTRA_OP_SET_SETTING: {
					// Takes effect right away (it lives in eepdata, which the screensaver and button code read);
					// reaches EEPROM on SAVE, or whenever eeprom_custom_save() runs. A value the setting does not
					// accept is BAD_ARGS and the setting keeps its old value.
					uint16_t value = (uint16_t)((data[4] << 8) | data[5]);
					uint8_t  stored; // the setting's EEPROM unit
					switch (data[3]) {
						case VIA_EXTRA_SETTING_SCREENSAVER_IDLE_MINUTES:
							if (value < SCREENSAVER_IDLE_MIN_MIN || value > SCREENSAVER_IDLE_MIN_MAX) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
							stored  = (uint8_t)value;
							changed = eepdata.screensaver_idle_min != stored;
							eepdata.screensaver_idle_min = stored;
							break;
						case VIA_EXTRA_SETTING_DIAL_SETTING_HOLD_MS:
							if (value != 1000 && value != 1500 && value != 2000 && value != 2500 && value != 3000) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
							stored  = (uint8_t)(value / 100); // eepdata keeps the hold time in 0.1 s units
							changed = eepdata.dial_hold_100ms != stored;
							eepdata.dial_hold_100ms = stored;
							break;
						default:
							*status = VIA_EXTRA_STATUS_BAD_ARGS;
							break;
					}
					break;
				}
			}
		}
		if (*status != VIA_EXTRA_STATUS_OK) used = 0;
		// Bytes after the response data are undefined (spec section 2) - clear them anyway
		if (3 + args + 1 + used < length) {
			memset(out + used, 0, length - (3 + args + 1 + used));
		}
		return changed;
	}

	// Unknown op: STATUS at byte 3 and nothing after it
	memset(&data[4], 0, length - 4);
	return false;
}
