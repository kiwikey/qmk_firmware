# Sofle CH582F (left half)

A [Sofle](https://github.com/josefadamcik/SofleKeyboard) left half, run as a
standalone single-unit keyboard (not split) with an RP2040 as the QMK MCU
and a CH582F as a BLE co-processor bridged over UART — see the
[qmk_firmware_ch582 project](../../../../../README.md) for the overall
architecture.

This target proves the working [ch582_bridge_test](../../ch582_bridge_test)
bring-up driver against a real matrix, real switches, and a rotary encoder
instead of the bring-up target's 2-key/1-row test rig. The right half and
split transport are not implemented yet — see the project's "2-topology
BLE" milestone for that.

* Keyboard Maintainer: qmk_firmware_ch582 project
* Hardware Supported: Sofle left half PCB + RP2040 + CH582F (WeAct CH582F
  board), wired by hand — not a drop-in Pro Micro replacement.
* Hardware Availability: not for distribution; project-internal test rig.

## Wiring

* RP2040 <-> CH582F bridge: UART0, GP0 (TX) / GP1 (RX) on the RP2040 to
  PB4 (RX) / PB7 (TX) on the CH582F, crossed. 921600 8N1. See
  `protocol/PROTOCOL.md` for the framing.
* Matrix: rows GP2-GP6, columns GP7-GP12 (see `keyboard.json`). These are
  free RP2040 GPIOs, not fixed by the Sofle PCB's own Pro Micro footprint —
  rewire `matrix_pins` in `keyboard.json` to match however you've actually
  wired the RP2040 to the PCB's row/column pads.
* Encoder: GP13 (A) / GP14 (B).
* Power: RP2040 and CH582F share a single 3.3V rail — do not power both
  boards from their own USB at the same time (see project notes).

## Bootloader

Enter the bootloader with the RESET button (double-tap on a genuine Pico
board), or hold **BOOT** while plugging in USB.

## Build

```bash
qmk compile -kb kiwikey/sofle_ch582f -km default
```

## Flashing

```bash
qmk flash -kb kiwikey/sofle_ch582f -km default
```
