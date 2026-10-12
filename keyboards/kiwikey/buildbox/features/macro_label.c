#include "macro_label.h"
#include "dynamic_keymap.h"
#include "display/qp_graphics.h"

// VIA saves the macro buffer in many small chunks - rebuilding after each one would
// re-scan the whole buffer needlessly. Same trailing-edge debounce as the theme color
// repaint in features/via_custom.c: each write pushes the rebuild further out.
#define MACRO_LABEL_REBUILD_DEBOUNCE_MS 250

#ifndef DYNAMIC_KEYMAP_MACRO_DELAY
#	define DYNAMIC_KEYMAP_MACRO_DELAY TAP_CODE_DELAY // same fallback as quantum/dynamic_keymap.c
#endif

static char macro_labels[DYNAMIC_KEYMAP_MACRO_COUNT][MACRO_LABEL_LENGTH + 1];
static char layer_names_custom[DYNAMIC_KEYMAP_LAYER_COUNT][LAYER_NAME_LENGTH + 1];
static bool            labels_loaded = false; // false until the first lookup (and again after VIA writes)
static deferred_token  rebuild_token = INVALID_DEFERRED_TOKEN;

static uint8_t macro_read_byte(uint32_t offset) {
	uint8_t c;
	dynamic_keymap_macro_get_buffer(offset, 1, &c);
	return c;
}

/***  The one place that knows the "[Name]" format - used by both the LCD label loader
	and the playback, so they can never disagree about what counts as a name.
	A macro has a name if it starts with "[" and a "]" comes before the macro ends (0x00)
	or a key action begins (0x01) - so a typed "[Lock" with no "]" never reads on into
	keycode bytes. Returns the offset just past the "]", or 'offset' itself if there is no name.
	'name' (optional, MACRO_LABEL_LENGTH + 1 bytes) receives the printable characters, cut at
	MACRO_LABEL_LENGTH; it is "" when there is no name or the brackets are empty ("[]").
***/
static uint32_t macro_parse_label(uint32_t offset, uint32_t end, char *name) {
	uint8_t n = 0;
	if (name) name[0] = 0;
	if (offset >= end || macro_read_byte(offset) != '[') return offset;
	for (uint32_t i = offset + 1; i < end; i++) {
		uint8_t c = macro_read_byte(i);
		if (c == ']') {
			if (name) name[n] = 0;
			return i + 1;
		}
		if (c == 0x00 || c == 0x01) break; // macro ended or key action began, no closing bracket
		if (name && c >= 0x20 && c < 0x7F && n < MACRO_LABEL_LENGTH) { // keep scanning past the limit to find "]", just drop the extra characters
			name[n++] = (char)c;
		}
	}
	if (name) name[0] = 0;
	return offset;
}

/***  Offset just past the "[#LAYERS]" marker if the macro at 'offset' starts with it,
	or 'offset' itself if not. Only meaningful for MACRO_LAYERS_SLOT.
***/
static uint32_t macro_layers_marker_end(uint32_t offset, uint32_t end) {
	static const char marker[] = MACRO_LAYERS_MARKER;
	for (uint8_t i = 0; i < sizeof(marker) - 1; i++) {
		if (offset + i >= end || macro_read_byte(offset + i) != (uint8_t)marker[i]) return offset;
	}
	return offset + sizeof(marker) - 1;
}

/***  Split "Default,Media,Mouse,RGB" (starting at 'offset', right after the marker) into
	the per-layer names. Reads until the macro ends (0x00) or a key action begins (0x01).
***/
static void macro_parse_layer_names(uint32_t offset, uint32_t end) {
	uint8_t layer = 0;
	uint8_t n     = 0;
	for (; offset < end && layer < DYNAMIC_KEYMAP_LAYER_COUNT; offset++) {
		uint8_t c = macro_read_byte(offset);
		if (c == 0x00 || c == 0x01) break;
		if (c == ',') { layer++; n = 0; continue; }
		if (c < 0x20 || c >= 0x7F) continue;
		if (n == 0 && c == ' ') continue; // "Default, Media": no leading space
		if (n < LAYER_NAME_LENGTH) layer_names_custom[layer][n++] = (char)c;
	}
}

/***  Scan the whole macro buffer once and cache every macro's "[Name]", plus the layer
	names if the last slot holds them.
	Buffer layout (see quantum/dynamic_keymap.c): macros are separated by 0x00.
***/
static void macro_label_load(void) {
	uint32_t size = dynamic_keymap_macro_get_buffer_size();
	uint32_t off  = 0;

	memset(macro_labels, 0, sizeof(macro_labels));
	memset(layer_names_custom, 0, sizeof(layer_names_custom));
	for (uint8_t id = 0; id < DYNAMIC_KEYMAP_MACRO_COUNT && off < size; id++) {
		uint32_t after_marker = (id == MACRO_LAYERS_SLOT) ? macro_layers_marker_end(off, size) : off;
		if (after_marker != off) { // the layer-name slot: not a macro, no label
			macro_parse_layer_names(after_marker, size);
			off = after_marker;
		} else {
			off = macro_parse_label(off, size, macro_labels[id]);
		}
		while (off < size) { // skip to the start of the next macro
			if (macro_read_byte(off++) == 0x00) break;
		}
	}
	labels_loaded = true;
}

const char *macro_label_get(uint8_t id) {
	if (id >= DYNAMIC_KEYMAP_MACRO_COUNT) return NULL;
	if (!labels_loaded) macro_label_load();
	return macro_labels[id][0] ? macro_labels[id] : NULL;
}

const char *macro_layer_name_get(uint8_t layer) {
	if (layer >= DYNAMIC_KEYMAP_LAYER_COUNT) return NULL;
	if (!labels_loaded) macro_label_load();
	return layer_names_custom[layer][0] ? layer_names_custom[layer] : NULL;
}

static uint32_t macro_label_rebuild_callback(uint32_t trigger_time, void *cb_arg) {
	rebuild_token = INVALID_DEFERRED_TOKEN;
	macro_label_load();
	// Macro labels and layer names are both on the idle screen. Whatever covers it (menu,
	// Breakout, tutorial, screensaver) calls ui_refresh() itself when it closes.
	if (ui_idle_screen_visible()) {
		ui_refresh();
	}
	return 0; // one-shot, don't requeue
}

void macro_label_invalidate(void) {
	if (rebuild_token == INVALID_DEFERRED_TOKEN ||
		!extend_deferred_exec(rebuild_token, MACRO_LABEL_REBUILD_DEBOUNCE_MS)) {
		rebuild_token = defer_exec(MACRO_LABEL_REBUILD_DEBOUNCE_MS, macro_label_rebuild_callback, NULL);
	}
}

/***  Macro playback that skips the "[Name]" prefix
	QMK's own dynamic_keymap_macro_send() (quantum/dynamic_keymap.c) is not weak, so this
	is a copy of it with one extra step: if the macro starts with a valid "[Name]",
	start playing after the "]". Everything else (the "mid-write" check, delays, 0x01
	key actions) is the same, because the same send_string engine plays it.
***/
static char macro_send_next(void *arg) {
	uint32_t *offset = (uint32_t *)arg;
	return (char)macro_read_byte((*offset)++);
}

void macro_label_send(uint8_t id) {
	if (id >= DYNAMIC_KEYMAP_MACRO_COUNT) return;

	uint32_t end = dynamic_keymap_macro_get_buffer_size();
	// If the last byte isn't 0 we are in the middle of a (possibly aborted) buffer write, do nothing
	if (macro_read_byte(end - 1) != 0) return;

	// Skip N null characters: 'offset' then points at the Nth macro
	uint8_t  slot   = id;
	uint32_t offset = 0;
	while (id > 0) {
		if (offset == end) return; // no Nth macro in the buffer
		if (macro_read_byte(offset) == 0) --id;
		++offset;
	}

	// The layer-name slot holds names, not keystrokes - never play it
	if (slot == MACRO_LAYERS_SLOT && macro_layers_marker_end(offset, end) != offset) return;

	offset = macro_parse_label(offset, end, NULL);
	send_string_with_delay_impl(macro_send_next, &offset, DYNAMIC_KEYMAP_MACRO_DELAY);
}
