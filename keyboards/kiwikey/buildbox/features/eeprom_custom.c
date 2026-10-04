#include "eeprom_custom.h"

#include <qp.h>
#include "features/knob_custom.h"
#include "sensor/sensors_handler.h"
#include "display/qp_graphics.h"
#include "display/widgets/qp_menu.h"
#include "display/widgets/qp_widget_layer.h"

// VIA reserves exactly VIA_EEPROM_CUSTOM_CONFIG_SIZE bytes for this struct (config.h);
// the dynamic keymap starts right after it, so a mismatch silently overlaps the two.
_Static_assert(sizeof(eeprom_custom_t) == VIA_EEPROM_CUSTOM_CONFIG_SIZE, "VIA_EEPROM_CUSTOM_CONFIG_SIZE (config.h) must equal sizeof(eeprom_custom_t)");

#define EEPROM_CUSTOM_ADDR ((void *)(VIA_EEPROM_CUSTOM_CONFIG_ADDR))

eeprom_custom_t eepdata;

const eeprom_custom_t eepdata_default = {
	0,                        // Layer 0
	1,                        // Boot animation enabled
	0,                        // LCD Timeout: 2min (shortest option, index 0 of display_timeout_seconds[])
	BACKLIGHT_DEFAULT_LEVEL,  // LCD Brightness default (10 = max)
	0,                        // Lighting Layers OFF
	{ 126, 210,  42,  84 },   // Lighting Layers' HUEs: Cyan - Magenta - Yellow - Green
	{ 255, 255, 255, 255 },   // Lighting Layers' SATs: maximum (255)
	1,                        // Knob special effect enabled
	KNOB_FUNC_VOLUME,         // Knob: Volume
	213,                      // Theme HUE default
	KNOB_SENSITIVITY_MEDIUM,  // Knob-function activation sensitivity default
	1,                        // Unbox tutorial: undone (show it on next boot)
	1,                        // Screensaver effect: "Rain 1" (matrix_rain_1) - index 0 is "OFF"
	{  13,   1,   22,   10 }, // Layer icons: Earth - Application - Boss - Calculator (layer_icon_pool_icon() indices, qp_widget_layer.c)
	EEPROM_CUSTOM_LAYOUT_VERSION // Layout version - see eeprom_custom.h
};

void eeprom_custom_save(void) {
	eeprom_update_block(&eepdata, EEPROM_CUSTOM_ADDR, sizeof(eeprom_custom_t));
}

void eeprom_custom_reset(void) {
	eepdata = eepdata_default;
	eeprom_custom_save();
}

void eeprom_custom_load(void) {
	eeprom_read_block(&eepdata, EEPROM_CUSTOM_ADDR, sizeof(eeprom_custom_t));

	// Blank/corrupt EEPROM, right after a factory/bootmagic reset, or a layout change
	if (eepdata.layout_version != EEPROM_CUSTOM_LAYOUT_VERSION) {
		eeprom_custom_reset();
	}

	if (eeprom_custom_validate()) {
		eeprom_custom_save(); // persist the repaired values
	}
}

// Every field that's used as an index or a fixed set of values - anything else
// (hues, sats) is valid at any value.
#define CLAMP_TO_DEFAULT(field, is_valid) do { if (!(is_valid)) { eepdata.field = eepdata_default.field; changed = true; } } while (0)

bool eeprom_custom_validate(void) {
	bool changed = false;

	CLAMP_TO_DEFAULT(active_layer,       eepdata.active_layer < DYNAMIC_KEYMAP_LAYER_COUNT);
	CLAMP_TO_DEFAULT(display_bootanim,   eepdata.display_bootanim <= 1);
	CLAMP_TO_DEFAULT(display_timeout,    eepdata.display_timeout < DISPLAY_TIMEOUT_COUNT);
	CLAMP_TO_DEFAULT(display_brightness, eepdata.display_brightness >= 1 && eepdata.display_brightness <= BACKLIGHT_LEVELS);
	CLAMP_TO_DEFAULT(lighting_layers,    eepdata.lighting_layers <= 1);
	CLAMP_TO_DEFAULT(knob_effect,        eepdata.knob_effect < KNOB_EFFECT_COUNT);
	CLAMP_TO_DEFAULT(knob_func,          eepdata.knob_func < KNOB_FUNC_COUNT);
	CLAMP_TO_DEFAULT(knob_sensitivity,   eepdata.knob_sensitivity < KNOB_SENSITIVITY_COUNT);
	CLAMP_TO_DEFAULT(unbox_tutorial,     eepdata.unbox_tutorial <= 1);
	CLAMP_TO_DEFAULT(screensaver_effect, eepdata.screensaver_effect < SCREEN_SAVER_MAXITEMS);
	for (uint8_t i = 0; i < EEPROM_CUSTOM_MAX_LAYERS; i++) {
		CLAMP_TO_DEFAULT(layer_icon[i],  eepdata.layer_icon[i] < LAYER_ICON_POOL_COUNT);
	}

	return changed;
}
