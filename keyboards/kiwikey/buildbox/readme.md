# BuildBox

A 16-key macropad by KiwiKey, built around an RP2040, with:

* a 320x240 SPI TFT (ST7789, or the original ILI9341) driven by Quantum Painter,
* a magnetic rotary knob (AS5600 sensor over I2C),
* 2 buttons below the screen,
* per-key + underglow + knob-ring RGB (WS2812, RGB Matrix).

* Keyboard Maintainer: [KiwiKey](https://github.com/KiwiKey)
* Hardware Supported: BuildBox v1.0 PCB (RP2040)
* Hardware Availability: KiwiKey

## Using it

* **Button 1 / Button 2** - previous / next layer.
* **Button 1 + Button 2 together** - open the Settings Menu (Button 1 = Exit, Button 2 = OK, knob = move/change).
* **Hold Button 1 for 3 s** - jump straight to Dial Settings.
* **Knob** - volume, vertical or horizontal scroll (Settings Menu > Dial Settings > Function).
* First boot shows a short tutorial; it can be replayed from the Settings Menu ("QUICK TUTORIAL").

Keymap, layer colors, knob, display and RGB settings are also editable in [VIA](https://usevia.app) - load `buildbox_via.json` under *Settings > Design*.

## Building

Make example for this keyboard (after setting up your build environment):

    make kiwikey/buildbox:default

Flashing example for this keyboard:

    make kiwikey/buildbox:default:flash

The TFT driver is picked by one line in `rules.mk`: enable exactly one of `QUANTUM_PAINTER_DRIVERS += st7789_spi` / `ili9341_spi`.

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix and plug in the keyboard
* **Settings Menu**: "BOOT TO DFU" (press OK twice)
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available

## Source layout

| Path | What's there |
|---|---|
| `buildbox.c/.h` | QMK keyboard hooks, custom keycodes |
| `matrix.c` | 4x4 matrix + 2 direct-pin buttons |
| `features/` | EEPROM settings (`eeprom_custom`), VIA custom menus (`via_custom`), knob RGB, combos, web mirror (`webhid_*`) |
| `sensor/` | AS5600 driver and knob handling |
| `display/` | Quantum Painter setup, shared drawing helpers, fonts/images (`resources/`) |
| `display/widgets/` | Idle-screen widgets, Settings Menu, tutorial, screensavers, Breakout |
| `web/` | Browser page that mirrors the TFT over WebHID |
