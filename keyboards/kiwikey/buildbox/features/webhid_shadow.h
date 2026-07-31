#pragma once

/*
 * Web HID Stream — capture layer.
 *
 * qp_graphics.c and every .c file under display/widgets/ already start with
 * `#include "display/qp_graphics.h"` before calling any QP primitive. That
 * header (one line, already approved) includes this one, which redirects
 * qp_rect/qp_flush/etc to wrappers below that draw on the real hardware
 * *and* mirror the same call over webhid_stream — so none of those files
 * need to change themselves.
 *
 * qp_roundrect is deliberately NOT shadowed: it's a custom function defined
 * in features/qp_custom_api.c, which also includes qp_graphics.h. Shadowing
 * its own name would rewrite its definition, not just its call-sites -
 * rounded rects (menu boxes) stay an unmirrored gap for now, see
 * webhid_stream.h's protocol doc.
 *
 * Disabled (WEBHID_STREAM_ENABLE undefined in webhid_stream.h): this header
 * expands to nothing, qp_* calls go straight to the real library, no
 * indirection at all.
 */

#include "webhid_stream.h"

#if defined(WEBHID_STREAM_ENABLE)

#define qp_rect(...)              webhid_qp_rect(__VA_ARGS__)
#define qp_flush(...)             webhid_qp_flush(__VA_ARGS__)
#define qp_clear(...)             webhid_qp_clear(__VA_ARGS__)
#define qp_drawtext(...)          webhid_qp_drawtext(__VA_ARGS__)
#define qp_drawtext_recolor(...)  webhid_qp_drawtext_recolor(__VA_ARGS__)
#define qp_drawimage(...)         webhid_qp_drawimage(__VA_ARGS__)
#define qp_drawimage_recolor(...) webhid_qp_drawimage_recolor(__VA_ARGS__)
#define qp_circle(...)            webhid_qp_circle(__VA_ARGS__)

bool    webhid_qp_rect(painter_device_t device, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint8_t hue, uint8_t sat, uint8_t val, bool filled);
bool    webhid_qp_flush(painter_device_t device);
bool    webhid_qp_clear(painter_device_t device);
int16_t webhid_qp_drawtext(painter_device_t device, uint16_t x, uint16_t y, painter_font_handle_t font, const char *str);
int16_t webhid_qp_drawtext_recolor(painter_device_t device, uint16_t x, uint16_t y, painter_font_handle_t font, const char *str, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg);
bool    webhid_qp_drawimage(painter_device_t device, uint16_t x, uint16_t y, painter_image_handle_t image);
bool    webhid_qp_drawimage_recolor(painter_device_t device, uint16_t x, uint16_t y, painter_image_handle_t image, uint8_t hue_fg, uint8_t sat_fg, uint8_t val_fg, uint8_t hue_bg, uint8_t sat_bg, uint8_t val_bg);
bool    webhid_qp_circle(painter_device_t device, uint16_t x, uint16_t y, uint16_t radius, uint8_t hue, uint8_t sat, uint8_t val, bool filled);

#endif // defined(WEBHID_STREAM_ENABLE)
