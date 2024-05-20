#define LOGO_WIDTH  24
#define LOGO_HEIGHT 24
static const char PROGMEM qmk_logo[LOGO_HEIGHT/8][LOGO_WIDTH] = {
    { 0x81,0x82,0x83,0x84 },
    { 0xa1,0xa2,0xa3,0xa4 },
    { 0xc1,0xc2,0xc3,0xc4 }
};
static void render_qmk_logo(uint8_t col, uint8_t row) {
    for (uint8_t i = 0; i < LOGO_HEIGHT/8; i++) {
        oled_set_cursor(col, row++);
        oled_write_P(qmk_logo[i], false);
    }
}