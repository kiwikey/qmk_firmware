#include "testf4.h"

#include <qp.h>
#include "print.h"
#include "spi_master.h"
#include "color.h"

#include "pikachu.qgf.h"
#include "pusheen_240px.qgf.h"

static painter_device_t       my_display;
static painter_image_handle_t my_image;
static painter_image_handle_t my_gif;
static deferred_token         my_anim;

void st7789_init(void) {
	// Create my_display
    my_display = qp_st7789_make_spi_device(
		ST7789_WIDTH,
		ST7789_HEIGHT,
		DISPLAY_CS_PIN,
		DISPLAY_DC_PIN,
		DISPLAY_RST_PIN,
		DISPLAY_SPI_DIVISOR,
		DISPLAY_SPI_MODE
	);
    qp_init(my_display, QP_ROTATION_90);   // Initialise the my_display
	qp_power(my_display, true);
}

void keyboard_post_init_kb(void) {
    st7789_init();

    qp_rect(my_display, 0, 0, 240 - 1,240 - 1, HSV_BLACK, true);
    qp_flush(my_display);
	
    qp_rect(my_display, 50, 50, 100,100, HSV_WHITE, true);
	qp_rect(my_display, 150, 50, 200,100, HSV_RED, true);
	qp_rect(my_display, 50, 150, 100,200, HSV_GREEN, true);
	qp_rect(my_display, 150,150, 200,200, HSV_BLUE, true);
	qp_circle(my_display, 125, 125, 20, HSV_GOLD, false);
    qp_flush(my_display);
	
	// blink LED to confirm things are done
    setPinOutput(C13);
	togglePin(C13);
	wait_ms(200);
	togglePin(C13);
	wait_ms(200);
	togglePin(C13);
	wait_ms(200);
	togglePin(C13);
	wait_ms(2000);
	
	my_image = qp_load_image_mem(gfx_pikachu);
	if (my_image != NULL) {
        qp_drawimage(my_display, 0, 0, my_image);
    }
	wait_ms(2000);
	
	my_gif = qp_load_image_mem(gfx_pusheen_240px);
	if (my_gif != NULL) {
        my_anim = qp_animate(my_display, 0, 0, my_gif);
    }	
}
