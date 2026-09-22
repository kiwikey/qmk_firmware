// Custom BLUETOOTH_DRIVER: sends QMK HID reports to the CH582F bridge
// firmware over UART0, framed per protocol/PROTOCOL.md + bridge_protocol.h.
// Same driver proven on the ch582_bridge_test bring-up target, carried over
// unmodified now that it's driving a real matrix.
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
            // No real battery ADC wired up yet — reply with a stub value so
            // the round trip is still exercised.
            uint8_t stub_percent = 100;
            bridge_send(MSG_BATTERY_LEVEL, &stub_percent, 1);
            break;
        }

        case MSG_ERROR:
        case MSG_RAW_HID_IN:
        default:
            // Not yet handled.
            break;
    }
}

void bluetooth_init(void) {
    uart_init(BRIDGE_BAUD);
    bridge_decoder_init(&rx_decoder);

    uint8_t version = BRIDGE_PROTOCOL_VERSION;
    bridge_send(MSG_HELLO, &version, 1);
}

void bluetooth_task(void) {
    uint8_t type, len;
    uint8_t payload[BRIDGE_MAX_PAYLOAD];

    while (uart_available()) {
        uint8_t byte = uart_read();
        if (bridge_decoder_feed(&rx_decoder, byte, &type, payload, &len)) {
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
