# FeLLa40

![FeLLa40](https://kiwikey.vn/media/fella40/fella40.jpg)

FeLLa40 is a 40% Alice-layout keyboard, with an 128*32px OLED screen and 2 rotary encoders.

* Keyboard Maintainer: [KiwiKey](https://github.com/kiwikey)
* Hardware Supported: FeLLa40 Hotswap PCB
* Hardware Availability: N/A

Make example for this keyboard (after setting up your build environment):

    make kiwikey/fella40:default

Flashing example for this keyboard:

    make kiwikey/fella40:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the top-left key and plug in the keyboard.
* **Physical reset button**: Briefly press the button on the back of the PCB.
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available.
