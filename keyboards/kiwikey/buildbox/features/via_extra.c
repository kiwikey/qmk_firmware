#include "via_extra.h"

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
#define VIA_EXTRA_STORAGE_VERSION 1
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
static struct {
	uint8_t len;
	char    name[VIA_EXTRA_NAME_FIELD + 1]; // always NUL-terminated
} custom[VIA_EXTRA_LAYERS];
static bool names_dirty = false; // changed since the last load/save

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

void via_extra_init(void) {
	via_extra_macro_load();

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
					out[1] = VIA_EXTRA_FEATURE_NAMES | VIA_EXTRA_FEATURE_ICONS | VIA_EXTRA_FEATURE_GET_STATE | VIA_EXTRA_FEATURE_SETTINGS | VIA_EXTRA_FEATURE_MACRO_NAMES; // 0x37
					out[2] = VIA_EXTRA_LAYERS;
					out[3] = VIA_EXTRA_NAME_MAX;
					out[4] = VIA_EXTRA_ICON_COUNT;
					out[5] = VIA_EXTRA_MACRO_COUNT;    // byte 9: only meaningful with features bit 5
					out[6] = VIA_EXTRA_MACRO_NAME_MAX; // byte 10
					used   = 7;
					break;
				}
				case VIA_EXTRA_OP_GET_LAYER_NAME: {
					uint8_t layer = data[3];
					if (layer >= VIA_EXTRA_LAYERS) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
					const char *name = via_extra_layer_name(layer);
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
					bool           ok    = layer < VIA_EXTRA_LAYERS && len <= VIA_EXTRA_NAME_MAX;
					for (uint8_t i = 0; ok && i < len; i++) { // only the first 'len' bytes count; the rest is padding
						ok = name[i] >= 0x20 && name[i] <= 0x7E;
					}
					if (!ok) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
					if (custom[layer].len != len || memcmp(custom[layer].name, name, len) != 0) {
						memset(&custom[layer], 0, sizeof(custom[layer]));
						custom[layer].len = len; // len = 0 clears the custom name: the built-in one is shown again
						memcpy(custom[layer].name, name, len);
						names_dirty = true;
						changed     = true;
						via_extra_request_repaint();
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
					if (layer >= VIA_EXTRA_LAYERS) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
					out[0] = (uint8_t)(eepdata.layer_icon[layer] + 1); // protocol id = pool index + 1, so never 0
					used   = 1;
					break;
				}
				case VIA_EXTRA_OP_SET_LAYER_ICON: {
					uint8_t layer = data[3];
					uint8_t icon  = data[4];
					if (layer >= VIA_EXTRA_LAYERS || icon > VIA_EXTRA_ICON_COUNT) { *status = VIA_EXTRA_STATUS_BAD_ARGS; break; }
					uint8_t pool_index = icon ? (uint8_t)(icon - 1) : eepdata_default.layer_icon[layer]; // 0 = restore the layer's default icon
					if (eepdata.layer_icon[layer] != pool_index) {
						eepdata.layer_icon[layer] = pool_index; // reaches EEPROM on SAVE, or whenever eeprom_custom_save() runs (spec section 4)
						changed = true;
						via_extra_request_repaint();
					}
					break;
				}
				case VIA_EXTRA_OP_SAVE: {
					bool ok = true;
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
