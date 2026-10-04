#include "quantum.h"

#define GRID_ROWS   (MATRIX_ROWS - 1) // rows 0..3 are the 4x4 grid; the last row is the 2 direct-pin buttons
#define DIRECT_ROW  (MATRIX_ROWS - 1)

static const pin_t col_pins[MATRIX_COLS] = { GP4, GP5, GP6, GP7 };
static const pin_t row_pins[GRID_ROWS]   = { GP0, GP1, GP2, GP3 };
#define DIRECT_PIN_1 GP23
#define DIRECT_PIN_2 GP24

// Only the row being scanned is driven (low); every other row is left as a
// pull-up input (high-impedance), like QMK's stock matrix. Driving idle rows
// push-pull HIGH would short a HIGH row pin to the LOW one through two pressed
// keys in the same column if the PCB has no diodes.
static void select_row(uint8_t row) {
	gpio_set_pin_output(row_pins[row]);
	gpio_write_pin_low(row_pins[row]);
}

static void unselect_row(uint8_t row) {
	gpio_set_pin_input_high(row_pins[row]);
}

void matrix_init_custom(void) {
	// Rows idle as pull-up inputs (see select_row()/unselect_row() above)
	for (uint8_t row = 0; row < GRID_ROWS; row++) {
		unselect_row(row);
	}

	// Configure column pins as pull-up inputs
	for (uint8_t col = 0; col < MATRIX_COLS; col++) {
		gpio_set_pin_input_high(col_pins[col]);
	}

	// Configure direct key as pull-up input
	gpio_set_pin_input_high(DIRECT_PIN_1);
	gpio_set_pin_input_high(DIRECT_PIN_2);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
	bool matrix_has_changed = false;

	for (uint8_t row = 0; row < GRID_ROWS; row++) {

		matrix_row_t row_value = 0;

		select_row(row);
		wait_us(30);

		// Scan the normal columns
		for (uint8_t col = 0; col < MATRIX_COLS; col++) {
			if (!gpio_read_pin(col_pins[col])) {
				row_value |= (1 << col);
			}
		}

		unselect_row(row);

		if (current_matrix[row] != row_value) {
			current_matrix[row] = row_value;
			matrix_has_changed = true;
		}

	}

	matrix_row_t direct_value = 0;

	// Extra direct keys on the last row
	if (!gpio_read_pin(DIRECT_PIN_1)) {
		direct_value |= 0x01;
	}
	if (!gpio_read_pin(DIRECT_PIN_2)) {
		direct_value |= 0x02;
	}

	if (current_matrix[DIRECT_ROW] != direct_value) {
		current_matrix[DIRECT_ROW] = direct_value;
		matrix_has_changed = true;
	}

	return matrix_has_changed;
}
