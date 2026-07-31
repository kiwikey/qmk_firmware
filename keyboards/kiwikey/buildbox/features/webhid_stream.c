#include "webhid_stream.h"

#if defined(WEBHID_STREAM_ENABLE)

#include "raw_hid.h"
#include "usb_descriptor.h" /* RAW_EPSIZE */
#include "via.h"            /* id_custom_set_value */
#include "display/qp_graphics.h"         /* ui_refresh() */
#include "display/widgets/qp_menu.h"     /* menu_state, NOT_IN_MENU */
#include "display/widgets/qp_widget_breakout.h" /* breakout_is_active() */

/*
 * raw_hid_send() blocks for up to 100ms if the host isn't draining the
 * endpoint (QMK's raw HID IN queue is only a few packets deep) - see
 * tmk_core/protocol/chibios/usb_main.c:send_report(). Calling it every
 * housekeeping tick while no browser tab is open would stall the whole
 * keyboard. So: `enabled` defaults to false (nothing is ever sent until
 * something explicitly arms it, e.g. a future host->device handshake),
 * and even while armed, flushes are time-throttled rather than tied to
 * how often housekeeping_task runs.
 */

/* Sized to comfortably hold one full ui_refresh() burst (title + status text
 * + all 16 keypad labels + the knob's 8 circles), which measures out to
 * roughly 450+ bytes queued near-instantly. At the old 256B size, anything
 * past that got silently dropped by the overflow policy below - the back
 * half of every full-screen redraw (last matrix row, knob widget) never
 * made it out. */
#define WEBHID_TX_BUFFER_SIZE 1024
/* Deliberately far slower than the 1ms USB interrupt interval allows: attempting
 * sends at 1kHz assumed the host could drain just as fast, but a browser's
 * WebHID read loop can't sustain that. Outpacing it keeps the tiny hardware
 * IN-queue full, so raw_hid_send() blocks (up to 100ms, see below) on nearly
 * every call. This throttle trades mirror framerate for a safety margin
 * against that risk - it did NOT turn out to be the cause of the blanked-TFT
 * bug (that was a channel-id collision with QMK's stock backlight channel,
 * see the HANDSHAKE section in webhid_stream.h), but the risk is real and
 * this guards against it regardless. */
#define WEBHID_MIN_FLUSH_INTERVAL_MS 20
#define WEBHID_PARTIAL_FLUSH_TIMEOUT_MS 25 /* latency bound on a trailing sub-32-byte message */
#define WEBHID_PING_TIMEOUT_MS 2000 /* no PING for this long -> auto-disarm, see HANDSHAKE doc */

static uint8_t  tx_buf[WEBHID_TX_BUFFER_SIZE];
static uint16_t tx_head  = 0; /* next free slot to write into */
static uint16_t tx_count = 0; /* bytes currently queued */

static bool     enabled           = false;
static uint32_t last_flush_time   = 0;
static uint32_t last_write_time   = 0;
static uint32_t last_ping_time    = 0;

static void tx_push(uint8_t byte) {
    tx_buf[(tx_head + tx_count) % WEBHID_TX_BUFFER_SIZE] = byte;
    tx_count++;
}

static void tx_pop_report(uint8_t out[RAW_EPSIZE]) {
    uint8_t n = tx_count < RAW_EPSIZE ? tx_count : RAW_EPSIZE;
    for (uint8_t i = 0; i < n; i++) {
        out[i] = tx_buf[tx_head];
        tx_head = (tx_head + 1) % WEBHID_TX_BUFFER_SIZE;
    }
    for (uint8_t i = n; i < RAW_EPSIZE; i++) {
        out[i] = OP_PAD;
    }
    tx_count -= n;
}

bool webhid_stream_send(webhid_opcode_t opcode, const uint8_t *payload, uint8_t len) {
    if (!enabled) {
        return false;
    }

    uint16_t total = 3 + (uint16_t)len; /* sync + opcode + len + payload */
    if (total > WEBHID_TX_BUFFER_SIZE - tx_count) {
        return false; /* would overflow the ring buffer - drop, mirror is best-effort */
    }

    tx_push(WEBHID_SYNC_BYTE);
    tx_push((uint8_t)opcode);
    tx_push(len);
    for (uint8_t i = 0; i < len; i++) {
        tx_push(payload[i]);
    }
    last_write_time = timer_read32();
    return true;
}

uint16_t webhid_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

bool webhid_stream_enabled(void) {
    return enabled;
}

void webhid_stream_set_enabled(bool new_enabled) {
    if (new_enabled == enabled) {
        return;
    }
    enabled = new_enabled;

    if (enabled) {
        tx_head  = 0;
        tx_count = 0;
        uint8_t hello[5] = {
            WEBHID_PROTOCOL_VERSION,
            (uint8_t)(ILI9341_WIDTH & 0xFF),
            (uint8_t)(ILI9341_WIDTH >> 8),
            (uint8_t)(ILI9341_HEIGHT & 0xFF),
            (uint8_t)(ILI9341_HEIGHT >> 8),
        };
        webhid_stream_send(OP_HELLO, hello, sizeof(hello));

        /* Streaming only mirrors deltas from here on - anything drawn before
         * this moment (e.g. at boot) was never sent. Repaint the idle screen
         * now so a fresh connection gets a complete picture immediately,
         * instead of only whatever happens to redraw naturally afterward.
         * Skipped while in the menu/Breakout so this can't clobber those
         * screens with the idle one - same guard sensors_handler.c uses. */
        if (menu_state == NOT_IN_MENU && !breakout_is_active()) {
            ui_refresh();
        }
    }
}

void webhid_stream_ping(void) {
    last_ping_time = timer_read32();
    webhid_stream_set_enabled(true);
}

void webhid_stream_handle_config(uint8_t command_id, uint8_t *value_id_and_data) {
    if (command_id != id_custom_set_value) {
        return; /* get_value/save not used on this channel yet */
    }
    switch (value_id_and_data[0]) {
        case WEBHID_VALUE_PING:
            webhid_stream_ping();
            break;
        case WEBHID_VALUE_STOP:
            webhid_stream_set_enabled(false);
            break;
    }
}

void keyboard_post_init_webhid_stream(void) {
    tx_head         = 0;
    tx_count        = 0;
    enabled         = false;
    last_flush_time = timer_read32();
    last_ping_time  = timer_read32();
}

/* raw_hid_send() can block up to 100ms if the host isn't draining the
 * endpoint - see the note at the top of this file. WEBHID_MIN_FLUSH_INTERVAL_MS
 * is a guess at a safe send rate; this is the actual safety net: measure
 * how long the call really took, and if it looks like it blocked (not just
 * ran a bit slow), disarm immediately rather than trust the guess. */
#define WEBHID_SEND_STALL_MS 40

static void send_report_guarded(uint8_t report[RAW_EPSIZE]) {
    uint32_t started = timer_read32();
    raw_hid_send(report, RAW_EPSIZE);
    if (timer_elapsed32(started) >= WEBHID_SEND_STALL_MS) {
        webhid_stream_set_enabled(false); /* host isn't keeping up - stop before it happens again */
        return;
    }
    last_flush_time = timer_read32();
}

void housekeeping_task_webhid_stream(void) {
    if (enabled && timer_elapsed32(last_ping_time) > WEBHID_PING_TIMEOUT_MS) {
        webhid_stream_set_enabled(false); /* no PING in time - assume the browser is gone */
    }

    if (!enabled || tx_count == 0) {
        return;
    }

    if (tx_count >= RAW_EPSIZE) {
        if (timer_elapsed32(last_flush_time) < WEBHID_MIN_FLUSH_INTERVAL_MS) {
            return;
        }
        uint8_t report[RAW_EPSIZE];
        tx_pop_report(report);
        send_report_guarded(report);
        return;
    }

    /* Small trailing message: bound its latency instead of waiting for
     * WEBHID_TX_BUFFER_SIZE bytes to accumulate. */
    if (timer_elapsed32(last_write_time) >= WEBHID_PARTIAL_FLUSH_TIMEOUT_MS) {
        uint8_t report[RAW_EPSIZE];
        tx_pop_report(report);
        send_report_guarded(report);
    }
}

#else /* !defined(WEBHID_STREAM_ENABLE) - single-define kill switch, all no-ops */

bool     webhid_stream_send(webhid_opcode_t opcode, const uint8_t *payload, uint8_t len) { return false; }
uint16_t webhid_rgb565(uint8_t r, uint8_t g, uint8_t b) { return 0; }
bool     webhid_stream_enabled(void) { return false; }
void     webhid_stream_set_enabled(bool enabled) {}
void     webhid_stream_ping(void) {}
void     webhid_stream_handle_config(uint8_t command_id, uint8_t *value_id_and_data) {}
void     keyboard_post_init_webhid_stream(void) {}
void     housekeeping_task_webhid_stream(void) {}

#endif
