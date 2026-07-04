#include "buildbox.h"

#include <qp.h>
// #include <lib/lib8tion/lib8tion.h>
#include "print.h"

#if defined(QUANTUM_PAINTER_ENABLE)
	#include "display/qp_graphics.h"
	#include "display/qp_includes.h"
	#include "display/qp_custom_api.h"
	#include "display/eeprom_custom.h"
	#include "display/widgets/qp_widget_matrix.h"
	#include "display/widgets/qp_widget_layer.h"
	#include "display/widgets/qp_widget_knob.h"
#endif // defined(QUANTUM_PAINTER_ENABLE)

void keyboard_post_init_kb(void) {
	keyboard_post_init_display();
	keyboard_post_init_user();
}

void housekeeping_task_kb(void) {

}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	process_record_display(keycode, record);
	return process_record_user(keycode, record);
}
