#include "webhid_shadow.h"

#if defined(WEBHID_STREAM_ENABLE)

#include <string.h>
#include "display/qp_includes.h" /* font/icon handles, for id lookup below */

/* webhid_shadow.h #defines qp_rect/qp_flush/etc to redirect widget code to
 * the wrappers in this file. Undo that here so the calls inside these
 * wrapper bodies reach the real QP library functions instead of recursing
 * into themselves. */
#undef qp_rect
#undef qp_flush
#undef qp_clear
#undef qp_drawtext
#undef qp_drawtext_recolor
#undef qp_drawimage
#undef qp_drawimage_recolor
#undef qp_circle

#define WEBHID_MAX_TEXT_LEN 64 /* longer strings are truncated on the wire, not on the real screen */

static void put_u16(uint8_t *buf, uint16_t v) {
    buf[0] = (uint8_t)(v & 0xFF);
    buf[1] = (uint8_t)(v >> 8);
}

static uint16_t hsv565(uint8_t hue, uint8_t sat, uint8_t val) {
    hsv_t hsv = {.h = hue, .s = sat, .v = val};
    rgb_t rgb = hsv_to_rgb(hsv);
    return webhid_rgb565(rgb.r, rgb.g, rgb.b);
}

static uint8_t font_id_for(painter_font_handle_t font) {
    if (font == thintel16) return 0;
    if (font == thintel32) return 1;
    if (font == font_oled) return 2;
    if (font == nanoplex16) return 3;
    if (font == nanoplex32) return 4;
    return 0xFF; /* unknown font - browser should skip/ignore */
}

static uint8_t icon_id_for(painter_image_handle_t image) {
    if (image == ico32_brightness) return 0;
    if (image == ico16_arrow_up) return 1;
    if (image == ico16_arrow_down) return 2;
    if (image == ico16_arrow_left) return 3;
    if (image == ico16_arrow_right) return 4;
    if (image == ico12_arrow_left) return 5;
    if (image == ico12_arrow_right) return 6;
    if (image == gif_bootup01) return 7;
    if (image == img_anya01) return 8;
    if (image == gif_nyan120px) return 9;
    if (image == gif_cat01) return 10;
    return 0xFF; /* unknown image - browser should skip/ignore */
}

static void send_text(uint16_t x, uint16_t y, painter_font_handle_t font, const char *str,
                       uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg,
                       uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
    uint8_t str_len = (uint8_t)strnlen(str, WEBHID_MAX_TEXT_LEN);
    uint8_t payload[10 + WEBHID_MAX_TEXT_LEN];

    put_u16(&payload[0], x);
    put_u16(&payload[2], y);
    payload[4] = font_id_for(font);
    put_u16(&payload[5], hsv565(hue_fg, sat_fg, val_fg));
    put_u16(&payload[7], hsv565(hue_bg, sat_bg, val_bg));
    payload[9] = str_len;
    memcpy(&payload[10], str, str_len);

    webhid_stream_send(OP_TEXT, payload, 10 + str_len);
}

bool webhid_qp_rect(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint8_t hue, uint8_t sat, uint8_t val, bool filled) {
    bool ok = qp_rect(device, left, top, right, bottom, hue, sat, val, filled);

    uint8_t payload[11];
    put_u16(&payload[0], left);
    put_u16(&payload[2], top);
    put_u16(&payload[4], right);
    put_u16(&payload[6], bottom);
    put_u16(&payload[8], hsv565(hue, sat, val));
    payload[10] = filled ? 1 : 0;
    webhid_stream_send(OP_RECT, payload, sizeof(payload));

    return ok;
}

bool webhid_qp_flush(painter_device_t device) {
    bool ok = qp_flush(device);
    webhid_stream_send(OP_FLUSH, NULL, 0);
    return ok;
}

bool webhid_qp_clear(painter_device_t device) {
    bool ok = qp_clear(device);
    uint8_t payload[2] = {0x00, 0x00}; /* black - always followed by a full qp_rect fill in this codebase */
    webhid_stream_send(OP_CLEAR, payload, sizeof(payload));
    return ok;
}

int16_t webhid_qp_drawtext(painter_device_t device, uint16_t x, uint16_t y, painter_font_handle_t font, const char *str) {
    int16_t ret = qp_drawtext(device, x, y, font, str);
    /* plain qp_drawtext carries no color info; approximate as white-on-black, see webhid_shadow.h */
    send_text(x, y, font, str, 0, 0, 255, 0, 0, 0);
    return ret;
}

int16_t webhid_qp_drawtext_recolor(painter_device_t device, uint16_t x, uint16_t y, painter_font_handle_t font, const char *str, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
    int16_t ret = qp_drawtext_recolor(device, x, y, font, str, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
    send_text(x, y, font, str, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);
    return ret;
}

bool webhid_qp_drawimage(painter_device_t device, uint16_t x, uint16_t y, painter_image_handle_t image) {
    bool ok = qp_drawimage(device, x, y, image);

    uint8_t payload[5];
    put_u16(&payload[0], x);
    put_u16(&payload[2], y);
    payload[4] = icon_id_for(image);
    webhid_stream_send(OP_ICON, payload, sizeof(payload));

    return ok;
}

bool webhid_qp_drawimage_recolor(painter_device_t device, uint16_t x, uint16_t y, painter_image_handle_t image, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg) {
    bool ok = qp_drawimage_recolor(device, x, y, image, hue_fg, sat_fg, val_fg, hue_bg, sat_bg, val_bg);

    /* tint colors not sent - browser paints its own stored asset for this icon_id as-is, see webhid_shadow.h */
    uint8_t payload[5];
    put_u16(&payload[0], x);
    put_u16(&payload[2], y);
    payload[4] = icon_id_for(image);
    webhid_stream_send(OP_ICON, payload, sizeof(payload));

    return ok;
}

bool webhid_qp_circle(painter_device_t device, uint16_t x, uint16_t y, uint16_t radius, uint8_t hue, uint8_t sat, uint8_t val, bool filled) {
    bool ok = qp_circle(device, x, y, radius, hue, sat, val, filled);

    uint8_t payload[9];
    put_u16(&payload[0], x);
    put_u16(&payload[2], y);
    put_u16(&payload[4], radius);
    put_u16(&payload[6], hsv565(hue, sat, val));
    payload[8] = filled ? 1 : 0;
    webhid_stream_send(OP_CIRCLE, payload, sizeof(payload));

    return ok;
}

#endif // defined(WEBHID_STREAM_ENABLE)
