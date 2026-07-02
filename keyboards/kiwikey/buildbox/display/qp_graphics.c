#include QMK_KEYBOARD_H

#if defined(QUANTUM_PAINTER_ENABLE)

#include "display/qp_includes.h"
#include "display/eeprom_custom.h"
#include "display/widgets/qp_widget_matrix.h"
#include "display/widgets/qp_widget_layer.h"
#include "display/widgets/qp_widget_knob.h"


layer_state_t layer_state_set_kb(layer_state_t state) {
		widget_layer_render(get_highest_layer(state));
	return state;
}

#endif // defined(QUANTUM_PAINTER_ENABLE)
