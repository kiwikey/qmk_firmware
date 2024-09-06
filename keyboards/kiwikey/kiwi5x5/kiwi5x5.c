#include "kiwi5x5.h"

#include "print.h"
#include "spi_master.h"
#include "color.h"

#if defined(QUANTUM_PAINTER_ENABLE)
	painter_device_t my_display;
#endif // defined(QUANTUM_PAINTER_ENABLE)

void keyboard_post_init_kb(void) {
	
#if defined(QUANTUM_PAINTER_ENABLE)
    my_display = qp_st7789_make_spi_device(
		ST7789_WIDTH,
		ST7789_HEIGHT,
		DISPLAY_CS_PIN,
		DISPLAY_DC_PIN,
		DISPLAY_RST_PIN,
		DISPLAY_SPI_DIVISOR,
		DISPLAY_SPI_MODE
	);
    qp_init(my_display, QP_ROTATION_0);
	qp_power(my_display, true);
	qp_clear(my_display);
	qp_rect(my_display, 0, 0, 239, 239, HSV_BLACK, true); // Fill screen by black color
	void keyboard_post_init_display(void);
    keyboard_post_init_display();
#endif // defined(QUANTUM_PAINTER_ENABLE)
		
	// Allow for user post-init
    keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {

#if defined(QUANTUM_PAINTER_ENABLE)
	bool process_record_display(uint16_t keycode, keyrecord_t *record);
    return process_record_display(keycode, record);
#endif // defined(QUANTUM_PAINTER_ENABLE)
	
	return process_record_user(keycode, record);
}

layer_state_t layer_state_set_user(layer_state_t state) {
	
#if defined(QUANTUM_PAINTER_ENABLE)
	void layer_state_set_display(void);
    layer_state_set_display();
#endif // defined(QUANTUM_PAINTER_ENABLE)
	
	return state;
}

#if defined(QUANTUM_PAINTER_ENABLE)
//----------------------------------------------------------
// UI Placeholder, implemented in themes

__attribute__((weak)) void housekeeping_task_display(void) {}

void housekeeping_task_kb(void) {
	housekeeping_task_display();
}
#endif // defined(QUANTUM_PAINTER_ENABLE)