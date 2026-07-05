/*** JUST CODES THAT ARE NOT USED, FOR TESTING SOMEWHERE, IF NEEDED ***/

// USED IN: process_record_kb
	printf("--- get_keycode_string --- \n");
	for (uint8_t i = 0;  i < MATRIX_ROWS; i++) {
		for (uint8_t j = 0;  j < MATRIX_COLS; j++) {
			printf("%10s", get_keycode_string(dynamic_keymap_get_keycode(0, i, j)));
		}
		printf("\n");
	}
	printf("--- keycode_to_string --- \n");
	for (uint8_t i = 0;  i < MATRIX_ROWS; i++) {
		for (uint8_t j = 0;  j < MATRIX_COLS; j++) {
			printf("%10s", keycode_to_string(dynamic_keymap_get_keycode(0, i, j)));
		}
		printf("\n");
	}
	printf("--- keycode --- \n");
	for (uint8_t i = 0;  i < MATRIX_ROWS; i++) {
		for (uint8_t j = 0;  j < MATRIX_COLS; j++) {
			printf("%10x", dynamic_keymap_get_keycode(0, i, j));
		}
		printf("\n");
	}
	printf("\n");

// USED IN: int16_t as5600read_angle(void)
    if(as5600_read(&data)) {
        if(!(data & MAGNET_DETECTED_MASK)) {
            print("\nMagnet not present!\n");
        } else if(data & MAGNET_WEAK_MASK) {
            print("\nMagnet too weak!\n");
        } else if(data & MAGNET_STRONG_MASK) {
            print("\nMagnet too strong!\n");
        }
    }