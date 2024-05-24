#ifndef OLED_LOGO
#define OLED_LOGO

#include "quantum.h"

#define LOGO_WIDTH  24
#define LOGO_HEIGHT 24

void render_qmk_logo(uint8_t col, uint8_t row);

#endif /* OLED_LOGO */

//TODO: change the lib name to sth like "OLED_IMAGE_RENDER"