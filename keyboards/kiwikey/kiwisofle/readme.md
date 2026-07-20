# Kiwisofle Keyboard

![KiwisofleKeyboard version 1](https://i.imgur.com/S5GTKth.jpeg)

Kiwisofle is 6×4+5 keys column-staggered split keyboard. Based on Lily58, Corne and Helix keyboards.

More details about the keyboard and build guides can be found here: [Sofle Keyboard Build Log and Guide](https://josefadamcik.github.io/SofleKeyboard)

* Keyboard Maintainer: [Josef Adamcik](https://josef-adamcik.cz) [Twitter:@josefadamcik](https://twitter.com/josefadamcik)  
* Hardware Supported: SofleKeyboard PCB, converted to RP2040 (via the [`rp2040_ce` converter](https://docs.qmk.fm/feature_converters#rp2040_ce), e.g. Elite-Pi, Liatris, Helios, or other RP2040 Community Edition compatible boards)
* Hardware Availability: [PCB & Case Data](https://github.com/josefadamcik/SofleKeyboard)

## Firmware Revisions
- `kiwikey/kiwisofle/rev1` is used for v1, v2, and RGB PCBs (**NOT** RGB PCBs purchased from [Keyhive](https://keyhive.xyz))
- `kiwikey/kiwisofle/keyhive` is used for PCBs purchased from [Keyhive](https://keyhive.xyz/shop/sofle)

Make example for this keyboard (after setting up your build environment):

    make kiwikey/kiwisofle/rev1:default
    make kiwikey/kiwisofle/keyhive:default

Flashing example for this keyboard:

    make kiwikey/kiwisofle/rev1:default:flash
    make kiwikey/kiwisofle/keyhive:default:flash

Press reset button on he keyboard when asked.

Disconnect the first half, connect the second one and repeat the process.

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix
* **Physical reset button**: Briefly press the button near the TRRS connector, or hold BOOTSEL while plugging in USB (RP2040-specific)
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
