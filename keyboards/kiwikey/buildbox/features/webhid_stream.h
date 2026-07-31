#pragma once

/*
 * Web HID Stream — transport layer for mirroring the TFT to a browser tab
 * (buildbox/web) over QMK's Raw HID, plus a reserved channel for future
 * device configuration from that same webpage.
 *
 * KILL SWITCH: comment out WEBHID_STREAM_ENABLE below and this whole feature
 * compiles down to no-op stubs. Nothing else needs to change.
 */
#define WEBHID_STREAM_ENABLE

#include "quantum.h"

/* =========================================================================
 * WIRE PROTOCOL v1  (device -> host, mirrored over Raw HID / RAW_EPSIZE=32B)
 * =========================================================================
 *
 * Raw HID here is a plain 32-byte-at-a-time BYTE STREAM, not one message
 * per report: messages are packed back-to-back across report boundaries,
 * and a report's unused tail (if any) is zero-padded. The browser buffers
 * incoming report bytes into a rolling queue and parses messages out of it.
 *
 * Message framing:
 *
 *     [0xA5 sync][opcode:1][len:1][payload: len bytes]
 *
 *   - 0xA5 ("sync byte") marks the start of every message. It is never a
 *     valid ordinary payload/pad byte at the position the parser expects a
 *     header, so a browser that attaches mid-stream (or a parser that gets
 *     confused for any reason) can resynchronize by scanning forward for
 *     0xA5 without needing an out-of-band reset. No escaping is needed
 *     inside payload bytes — once a header is parsed, exactly `len` bytes
 *     are consumed as opaque payload regardless of their value.
 *   - `len` is 0-255, payload length in bytes only (header itself is 3B).
 *   - Padding between/after messages is any non-0xA5 filler (firmware uses
 *     0x00); the parser just skips bytes while hunting for the next 0xA5.
 *   - All multi-byte fields are little-endian (native for RP2040 and for
 *     JS DataView with `littleEndian: true`).
 *   - Colors on the wire are RGB565 (2 bytes), matching the ILI9341's
 *     native format — smaller than sending HSV/RGB888 and what the browser
 *     needs to paint pixels anyway.
 *
 * This is a best-effort mirror, not a reliable protocol: a dropped/garbled
 * message just causes a momentary visual glitch, corrected by the next
 * redraw. There is no ack/retransmit.
 *
 * Opcodes (only PAD/HELLO/CLEAR/RECT/TEXT/ICON/CIRCLE/FLUSH are meaningful
 * today; the rest are reserved so numbering doesn't shift later):
 *
 *   0x00  OP_PAD     len=0             stream filler, never a real message
 *   0x01  OP_HELLO   len=5             [proto_ver:1][width:2][height:2]
 *                                      sent once when streaming is (re)armed
 *   0x02  OP_CLEAR   len=2             [color:2]  fill whole screen
 *   0x10  OP_RECT    len=11            [x0:2][y0:2][x1:2][y1:2][color:2][filled:1]
 *   0x11  OP_TEXT    len=10+N          [x:2][y:2][font_id:1][fg:2][bg:2][str_len:1][str:N]
 *   0x12  OP_ICON    len=5             [x:2][y:2][icon_id:1]
 *   0x13  OP_CIRCLE  len=9             [x:2][y:2][radius:2][color:2][filled:1]
 *   0x20  OP_FLUSH   len=0             end-of-frame marker (present/composite point)
 *   0xFF  OP_RESET   len=0             ask the browser to drop state and re-sync
 *
 * 0x03-0x0F, 0x14-0x1F, 0x21-0xFE are reserved for growth (pixel-run/RLE
 * fallback, GIF-frame blit, cursor, etc).
 *
 * NOT mirrored in v1: qp_roundrect() (menu boxes) draws via internal QP
 * fill-helpers, bypassing qp_rect entirely, and it's a function this
 * codebase defines itself (features/qp_custom_api.c) rather than calls -
 * shadowing its name would rewrite its own definition. Left as a known gap;
 * see features/webhid_shadow.h.
 *
 * =========================================================================
 * HANDSHAKE  (host -> device, rides QMK's VIA custom-value channel)
 * =========================================================================
 *
 * features/via_custom.c already owns raw_hid_receive() (via quantum/via.c)
 * and dispatches incoming id_custom_set_value/get_value/save reports by a
 * `channel_id` byte; channel 0 is its own existing config. This module
 * reserves WEBHID_CONFIG_CHANNEL_ID and forwards anything on that channel,
 * unparsed, to webhid_stream_handle_config() below — via_custom.c doesn't
 * need to know what PING/STOP mean, just that this channel is ours.
 *
 * IMPORTANT: quantum/via.c's own via_custom_value_command() checks QMK's
 * *stock* channel ids (enum via_channel_id in quantum/via.h) BEFORE a
 * report ever reaches via_custom_value_command_kb() - i.e. before
 * via_custom.c's dispatch, and before this file, get a look at it at all.
 * Those stock ids currently run 0 (id_custom_channel) through 5
 * (id_qmk_led_matrix_channel). WEBHID_CONFIG_CHANNEL_ID must stay clear of
 * that whole range, or a PING can get silently reinterpreted as e.g. a
 * backlight command (this happened: channel 1 collided with
 * id_qmk_backlight_channel, and value_id 1 / value 0 == "set backlight
 * brightness to zero" == blanked TFT, entirely inside unmodified QMK core
 * code, never reaching a single line we wrote).
 *
 * Report layout (32B OUT report, id_custom_set_value = 0x07):
 *   [0]=0x07  [1]=WEBHID_CONFIG_CHANNEL_ID  [2]=value_id  [3..]=unused
 *
 * Value ids:
 *   WEBHID_VALUE_PING (0x01) - browser sends every ~500ms while its tab is
 *     open and reading. Arms streaming (idempotent if already armed) and
 *     resets the watchdog below.
 *   WEBHID_VALUE_STOP (0x02) - browser sends on a clean disconnect/tab
 *     close. Disarms streaming immediately. Purely a courtesy for a fast
 *     stop - NOT relied on, since a crash/unplug/frozen tab can't send it.
 *
 * The real safety net is the watchdog in housekeeping_task_webhid_stream():
 * if streaming is armed and no PING has arrived for WEBHID_PING_TIMEOUT_MS,
 * it disarms on its own. This is what keeps raw_hid_send() (100ms block
 * risk, see webhid_stream.c) from ever being called at a dead endpoint -
 * the module self-heals from any disconnect, clean or not, without
 * depending on the browser telling it anything.
 */

#define WEBHID_SYNC_BYTE 0xA5

typedef enum {
    OP_PAD    = 0x00,
    OP_HELLO  = 0x01,
    OP_CLEAR  = 0x02,
    OP_RECT   = 0x10,
    OP_TEXT   = 0x11,
    OP_ICON   = 0x12,
    OP_CIRCLE = 0x13,
    OP_FLUSH  = 0x20,
    OP_RESET  = 0xFF,
} webhid_opcode_t;

#define WEBHID_PROTOCOL_VERSION 1
#define WEBHID_CONFIG_CHANNEL_ID 0x20 /* clear of QMK's reserved 0-5 range, see HANDSHAKE above */

#define WEBHID_VALUE_PING 0x01
#define WEBHID_VALUE_STOP 0x02

/* ---- lifecycle (call sites live in buildbox.c, added only after sign-off) ---- */
void keyboard_post_init_webhid_stream(void);
void housekeeping_task_webhid_stream(void);

/* ---- runtime control ---- */
bool webhid_stream_enabled(void);
void webhid_stream_set_enabled(bool enabled);
void webhid_stream_ping(void);

/* ---- host->device dispatch entry point, called from via_custom.c for
 * anything arriving on WEBHID_CONFIG_CHANNEL_ID; command_id is VIA's
 * top-level id_custom_set_value/get_value/save byte. ---- */
void webhid_stream_handle_config(uint8_t command_id, uint8_t *value_id_and_data);

/* ---- low-level send: frames and queues one message; non-blocking, drops
 * on overflow. Returns false if the message didn't fit and was dropped. ---- */
bool webhid_stream_send(webhid_opcode_t opcode, const uint8_t *payload, uint8_t len);

/* ---- helper: pack 8-bit RGB into the wire's RGB565 ---- */
uint16_t webhid_rgb565(uint8_t r, uint8_t g, uint8_t b);
