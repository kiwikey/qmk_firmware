#include "kiwi5x5.h"

#include "print.h"
#include "spi_master.h"
#include "color.h"

#if defined(QUANTUM_PAINTER_ENABLE)
	painter_device_t my_display;
#endif // defined(QUANTUM_PAINTER_ENABLE)

void keyboard_post_init_kb(void) {
	
#if defined(QUANTUM_PAINTER_ENABLE)
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