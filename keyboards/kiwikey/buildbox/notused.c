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