#pragma once

#include "quantum.h"

// User-named macros: a macro whose text starts with "[Name]" (typed in VIA's macro
// editor) shows "Name" on the LCD instead of "MACRO <n>". Only the first
// MACRO_LABEL_LENGTH characters are shown; longer names are truncated.

#define MACRO_LABEL_LENGTH 5

// Name of macro 'id' (0-based), or NULL when it has no "[...]" name (or id is out of range)
const char *macro_label_get(uint8_t id);

// User-named layers: the LAST macro slot can hold all the layer names instead of a macro:
//     [#LAYERS]Default,Media,Mouse,RGB
// (comma-separated, in layer order). A blank or missing name falls back to the built-in
// one. The slot only counts as the layer-name slot while it starts with that marker,
// and then it is never played as a macro.
#define MACRO_LAYERS_SLOT   (DYNAMIC_KEYMAP_MACRO_COUNT - 1)
#define MACRO_LAYERS_MARKER "[#LAYERS]"
#define LAYER_NAME_LENGTH   10 // longer names are truncated - what fits in the layer box on the LCD

// Custom name of layer 'layer', or NULL to use the built-in one (or layer is out of range)
const char *macro_layer_name_get(uint8_t layer);

// Call when VIA writes or clears the macro buffer. Debounced: the caches are rebuilt
// (and the idle screen redrawn) once VIA has stopped writing.
void macro_label_invalidate(void);

// Plays macro 'id' like QMK's dynamic_keymap_macro_send(), but without typing its "[Name]" prefix
void macro_label_send(uint8_t id);
