// Custom BLUETOOTH_DRIVER: sends QMK HID reports to the CH582F bridge
// firmware over UART0, framed per protocol/PROTOCOL.md + bridge_protocol.h.
// This is the RP2040 side's minimal bring-up counterpart to the CH582F
// side's examples/HID_Keyboard bridge adaptation.
//
// Scope note: only keyboard/mouse/consumer/system reports are wired up,
// matching what the CH582F side currently implements. NKRO and raw HID are
// left as weak (no-op) per drivers/bluetooth/bluetooth.c defaults —
// bluetooth_send_nkro()/bluetooth_send_raw_hid() are intentionally not
// overridden here.

#include "bluetooth.h"
#include "uart.h"
#include "report.h"
#include "bridge_protocol.h"
#include "print.h" // TEMPORARY: debug visibility while bringing the link up

#include <string.h>

#define BRIDGE_BAUD 921600

static bridge_decoder_t rx_decoder;
static uint8_t          link_state = LINK_STATE_IDLE;
static uint8_t          host_leds  = 0;

static void bridge_send(uint8_t type, const uint8_t *payload, uint8_t len) {
    uint8_t encoded[2 * (BRIDGE_MAX_PAYLOAD + 3) + 2];
    size_t  encoded_len = bridge_encode(type, payload, len, encoded, sizeof(encoded));
    uart_transmit(encoded, (uint16_t)encoded_len);
}

static void bridge_handle_frame(uint8_t type, const uint8_t *payload, uint8_t len) {
    switch (type) {
        case MSG_HELLO_ACK:
            // payload: version(1), fw_major(1), fw_minor(1) — nothing to do
            // with it yet beyond having received it; a version mismatch
            // check can be added here once this matters in practice.
            break;

        case MSG_LINK_STATUS:
            if (len >= 1) {
                link_state = payload[0];
            }
            break;

        case MSG_LED_STATE:
            if (len >= 1) {
                host_leds = payload[0];
            }
            break;

        case MSG_BATTERY_REQUEST: {
            // No real battery ADC wired up in this bring-up target yet —
            // reply with a stub value so the round trip is still exercised.
            uint8_t stub_percent = 100;
            bridge_send(MSG_BATTERY_LEVEL, &stub_percent, 1);
            break;
        }

        case MSG_ERROR:
        case MSG_RAW_HID_IN:
        default:
            // Not yet handled in this bring-up target.
            break;
    }
}

void bluetooth_init(void) {
    uprintf("ch582_bridge: bluetooth_init, uart_init(%d)\n", BRIDGE_BAUD);
    uart_init(BRIDGE_BAUD);
    bridge_decoder_init(&rx_decoder);

    uint8_t version = BRIDGE_PROTOCOL_VERSION;
    bridge_send(MSG_HELLO, &version, 1);
    uprintf("ch582_bridge: sent MSG_HELLO (version=%d)\n", version);
}

void bluetooth_task(void) {
    uint8_t type, len;
    uint8_t payload[BRIDGE_MAX_PAYLOAD];

    while (uart_available()) {
        uint8_t byte = uart_read();
        uprintf("ch582_bridge: rx byte 0x%02X\n", byte);
        if (bridge_decoder_feed(&rx_decoder, byte, &type, payload, &len)) {
            uprintf("ch582_bridge: rx frame type=0x%02X len=%d\n", type, len);
            bridge_handle_frame(type, payload, len);
        }
    }
}

bool bluetooth_is_connected(void) {
    return link_state == LINK_STATE_CONNECTED;
}

uint8_t bluetooth_keyboard_leds(void) {
    return host_leds;
}

void bluetooth_send_keyboard(report_keyboard_t *report) {
    uint8_t payload[8];
    payload[0] = report->mods;
    payload[1] = report->reserved;
    memcpy(&payload[2], report->keys, 6);
    uprintf("ch582_bridge: send_keyboard mods=0x%02X keys=%02X %02X %02X %02X %02X %02X\n",
            report->mods, report->keys[0], report->keys[1], report->keys[2],
            report->keys[3], report->keys[4], report->keys[5]);
    bridge_send(MSG_KEYBOARD_REPORT, payload, sizeof(payload));
}

void bluetooth_send_mouse(report_mouse_t *report) {
    uint8_t payload[5];
    payload[0] = report->buttons;
    payload[1] = (uint8_t)report->x;
    payload[2] = (uint8_t)report->y;
    payload[3] = (uint8_t)report->v;
    payload[4] = (uint8_t)report->h;
    bridge_send(MSG_MOUSE_REPORT, payload, sizeof(payload));
}

void bluetooth_send_consumer(uint16_t usage) {
    uint8_t payload[2] = {(uint8_t)(usage & 0xFF), (uint8_t)(usage >> 8)};
    bridge_send(MSG_CONSUMER_REPORT, payload, sizeof(payload));
}

void bluetooth_send_system(uint16_t usage) {
    uint8_t payload[2] = {(uint8_t)(usage & 0xFF), (uint8_t)(usage >> 8)};
    bridge_send(MSG_SYSTEM_REPORT, payload, sizeof(payload));
}
