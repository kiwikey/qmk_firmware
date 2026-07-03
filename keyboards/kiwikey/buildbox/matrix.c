#include "quantum.h"

static const pin_t col_pins[4] = { GP0, GP1, GP2, GP3 };
static const pin_t row_pins[4] = { GP4, GP5, GP6, GP7 };
#define DIRECT_PIN GP24

void matrix_init_custom(void) {
    // Configure row pins as outputs (inactive = HIGH)
    for (int row = 0; row < 4; row++) {
        gpio_set_pin_output(row_pins[row]);
        gpio_write_pin_high(row_pins[row]);
    }

    // Configure column pins as pull-up inputs
    for (int col = 0; col < 4; col++) {
        gpio_set_pin_input_high(col_pins[col]);
    }

    // Configure direct key as pull-up input
    gpio_set_pin_input_high(DIRECT_PIN);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool matrix_has_changed = false;

    for (int row = 0; row < 4; row++) {

        matrix_row_t row_value = 0;

        gpio_write_pin_low(row_pins[row]);
        wait_us(30);

        // Scan 4 normal columns
        for (int col = 0; col < 4; col++) {
            if (!gpio_read_pin(col_pins[col])) {
                row_value |= (1 << col);
            }
        }

        gpio_write_pin_high(row_pins[row]);

        if (current_matrix[row] != row_value) {
            current_matrix[row] = row_value;
            matrix_has_changed = true;
        }

    }

    matrix_row_t row4_value = 0;

    // Extra direct key on ROW4 COL0
    if (!gpio_read_pin(DIRECT_PIN)) {
        row4_value |= 1;
    }

    if (current_matrix[4] != row4_value) {
        current_matrix[4] = row4_value;
        matrix_has_changed = true;
    }

    return matrix_has_changed;
}