// RP2040 <-> CH582F bridge protocol v1 — see PROTOCOL.md for the full spec.
// Copy this file unmodified into both firmware trees. Plain C99, no
// framework dependencies, so it compiles under both QMK's build and WCH's
// toolchain.
#pragma once

#include <stdint.h>
#include <stddef.h>

#define BRIDGE_PROTOCOL_VERSION 1

#define BRIDGE_FRAME_FLAG  0x7E
#define BRIDGE_FRAME_ESC   0x7D
#define BRIDGE_FRAME_ESC_XOR 0x20

#define BRIDGE_MAX_PAYLOAD 60

// RP2040 -> CH582F
#define MSG_HELLO             0x01
#define MSG_KEYBOARD_REPORT   0x02
#define MSG_NKRO_REPORT       0x03
#define MSG_MOUSE_REPORT      0x04
#define MSG_CONSUMER_REPORT   0x05
#define MSG_SYSTEM_REPORT     0x06
#define MSG_RAW_HID           0x07
#define MSG_ENTER_PAIRING     0x10
#define MSG_CLEAR_BONDS       0x11
#define MSG_SELECT_PROFILE    0x12
#define MSG_SET_POWER_MODE    0x13
#define MSG_SET_DEVICE_NAME   0x14
#define MSG_BATTERY_LEVEL     0x15
#define MSG_TUNNEL            0x20 // reserved, both directions — see PROTOCOL.md

// CH582F -> RP2040
#define MSG_HELLO_ACK         0x81
#define MSG_LINK_STATUS       0x82
#define MSG_LED_STATE         0x83
#define MSG_BATTERY_REQUEST   0x84
#define MSG_RAW_HID_IN        0x85
#define MSG_ERROR             0x86

// MSG_LINK_STATUS state values
#define LINK_STATE_IDLE        0x00 // advertising, not connected
#define LINK_STATE_CONNECTED   0x01
#define LINK_STATE_BONDED_IDLE 0x02

// MSG_ERROR code values
#define BRIDGE_ERR_CRC       0x01
#define BRIDGE_ERR_TYPE      0x02
#define BRIDGE_ERR_LENGTH    0x03

#define BRIDGE_CLEAR_BONDS_ALL 0xFF

// CRC-8/SMBUS (poly 0x07, init 0x00, no reflection, no xorout), computed
// over LEN|TYPE|PAYLOAD. Table-driven for speed; both MCUs can afford 256
// bytes of const data.
static const uint8_t bridge_crc8_table[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3,
};

static inline uint8_t bridge_crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; i++) {
        crc = bridge_crc8_table[crc ^ data[i]];
    }
    return crc;
}

// Streaming byte-stuffed frame decoder. Feed it one received byte at a
// time; it returns 1 and fills out type/payload/payload_len when a
// complete, CRC-valid frame has been decoded, 0 otherwise. On CRC mismatch
// the frame is silently dropped (state resets, caller may bump an error
// counter itself). Caller owns `buf` (>= BRIDGE_MAX_PAYLOAD + 2 bytes: LEN,
// TYPE, PAYLOAD) and the decoder struct's lifetime.
typedef struct {
    uint8_t buf[BRIDGE_MAX_PAYLOAD + 3]; // LEN + TYPE + PAYLOAD + CRC
    uint8_t idx;
    uint8_t escape_next;
    uint8_t in_frame;
} bridge_decoder_t;

static inline void bridge_decoder_init(bridge_decoder_t *dec) {
    dec->idx = 0;
    dec->escape_next = 0;
    dec->in_frame = 0;
}

static inline int bridge_decoder_feed(bridge_decoder_t *dec, uint8_t byte, uint8_t *type_out, uint8_t *payload_out, uint8_t *payload_len_out) {
    if (byte == BRIDGE_FRAME_FLAG) {
        // A flag byte both ends a frame in progress and starts the next one.
        int decoded = 0;
        if (dec->in_frame && dec->idx >= 3) {
            uint8_t  len       = dec->buf[0];
            uint8_t  frame_len = (uint8_t)(dec->idx - 1); // exclude trailing CRC byte
            if (len == frame_len - 1 && bridge_crc8(dec->buf, frame_len) == dec->buf[frame_len]) {
                *type_out        = dec->buf[1];
                *payload_len_out = (uint8_t)(len - 1);
                for (uint8_t i = 0; i < *payload_len_out; i++) {
                    payload_out[i] = dec->buf[2 + i];
                }
                decoded = 1;
            }
        }
        dec->idx       = 0;
        dec->escape_next = 0;
        dec->in_frame  = 1; // this flag byte doubles as the next frame's start
        return decoded;
    }

    if (!dec->in_frame) {
        return 0; // waiting for a start-of-frame flag
    }

    if (byte == BRIDGE_FRAME_ESC) {
        dec->escape_next = 1;
        return 0;
    }

    if (dec->escape_next) {
        byte ^= BRIDGE_FRAME_ESC_XOR;
        dec->escape_next = 0;
    }

    if (dec->idx < sizeof(dec->buf)) {
        dec->buf[dec->idx++] = byte;
    } else {
        // Overflow: drop the malformed frame and wait for the next flag byte.
        dec->in_frame = 0;
    }
    return 0;
}

// Encodes one frame into `out` (caller-provided, must be large enough:
// worst case every byte escaped is 2*(len_byte+type+payload+crc) + 2 flags).
// Returns the number of bytes written.
static inline size_t bridge_encode(uint8_t type, const uint8_t *payload, uint8_t payload_len, uint8_t *out, size_t out_size) {
    uint8_t raw[BRIDGE_MAX_PAYLOAD + 2];
    raw[0] = (uint8_t)(payload_len + 1);
    raw[1] = type;
    for (uint8_t i = 0; i < payload_len; i++) {
        raw[2 + i] = payload[i];
    }
    uint8_t raw_len       = (uint8_t)(payload_len + 2);
    uint8_t crc           = bridge_crc8(raw, raw_len);

    size_t out_idx = 0;
    if (out_idx < out_size) out[out_idx++] = BRIDGE_FRAME_FLAG;

    for (uint8_t i = 0; i <= raw_len; i++) {
        uint8_t b = (i < raw_len) ? raw[i] : crc;
        if (b == BRIDGE_FRAME_FLAG || b == BRIDGE_FRAME_ESC) {
            if (out_idx < out_size) out[out_idx++] = BRIDGE_FRAME_ESC;
            b ^= BRIDGE_FRAME_ESC_XOR;
        }
        if (out_idx < out_size) out[out_idx++] = b;
    }

    if (out_idx < out_size) out[out_idx++] = BRIDGE_FRAME_FLAG;
    return out_idx;
}
