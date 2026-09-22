#pragma once

// UART0 pins for the bridge link to the CH582F. Wire RP2040 GP0 (TX) to
// CH582F PB4 (RX) and RP2040 GP1 (RX) to CH582F PB7 (TX) — crossed, as
// usual for UART. 921600 baud 8N1, set in ch582_bridge.c to match
// protocol/PROTOCOL.md.
#define UART_DRIVER SIOD0
#define UART_TX_PIN GP0
#define UART_RX_PIN GP1

#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 200U  // ms window between taps; optional, this is the default
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP17      // optional: a GPIO to blink while in bootloader

#define I2C1_SDA_PIN GP2
#define I2C1_SCL_PIN GP3

#define QUANTUM_PAINTER_DISPLAY_TIMEOUT 0  // e.g. 60s, or 0 to disable
