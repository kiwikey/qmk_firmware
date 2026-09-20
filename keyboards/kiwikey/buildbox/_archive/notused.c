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

// USED IN: qp_widget_matrix.c
	void widget_matrix_render_singlebutton(uint8_t x, uint8_t y, uint8_t hue, uint8_t sat, uint8_t val, bool text_on, uint8_t layer) {
		// Button outline
		qp_roundrect(my_display,
					WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING), // left
					WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING), // top
					WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH, // right
					WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT, // bottom
					hue, sat, val,
					false,
					WIDGET_MATRIX_KEY_CORNER, true, true);
		// Inner text
		if (text_on) {
			widget_matrix_bgclear_singlebutton(x, y);
			uint16_t x_offset = WIDGET_MATRIX_POSX + y* (WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH/2;
			uint16_t y_offset = WIDGET_MATRIX_POSY + x* (WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT/2;
			uint16_t keycode = dynamic_keymap_get_keycode(layer, x, y);
			switch (keycode) {
				case QK_MOMENTARY ... QK_PERSISTENT_DEF_LAYER_MAX: // All layer-related keycodes (0x5220 to 0x52FF)
					widget_matrix_render_kc_layer(x_offset, y_offset, keycode);
					break;
				case BASIC_KEYCODE_RANGE:
				case MODIFIER_KEYCODE_RANGE:
				case USER_KEYCODE_RANGE:
					widget_matrix_render_kc_basic(x_offset, y_offset, keycode);
					break;
				default:
					widget_matrix_render_kc_basic(x_offset, y_offset, keycode);
					// ;
			}
		}
	}

	void widget_matrix_bgclear_singlebutton(uint8_t x, uint8_t y) { // just inner button, has no effect to button's outline
	qp_roundrect(my_display,
				WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + 1, // left
				WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + 1, // top
				WIDGET_MATRIX_POSX + y*(WIDGET_MATRIX_KEY_WIDTH  + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_WIDTH - 1, // right
				WIDGET_MATRIX_POSY + x*(WIDGET_MATRIX_KEY_HEIGHT + WIDGET_MATRIX_KEY_SPACING) + WIDGET_MATRIX_KEY_HEIGHT - 1, // bottom
				WIDGET_MATRIX_BUTTON_BG,
				true,
				WIDGET_MATRIX_KEY_CORNER, true, true
			);
	}

	void widget_matrix_render_kc_layer(uint16_t posx, uint16_t posy, uint16_t keycode) {
	// Top-left label background, TODO: clean this up
		qp_rect(my_display,
				posx - WIDGET_MATRIX_KEY_WIDTH/2 + 1,
				posy - WIDGET_MATRIX_KEY_WIDTH/2 + 1,
				posx - WIDGET_MATRIX_KEY_WIDTH/2 + 16,
				posy - WIDGET_MATRIX_KEY_WIDTH/2 + WIDGET_MATRIX_LABEL_FONT->line_height + 2,
				WIDGET_MATRIX_LABEL_BG, true);
	// Type of layer-keycode
		char buf1[4], buf2[10]; // TODO: optimize buf1 buf2 length
		switch (keycode) {
			case QK_MOMENTARY ... QK_MOMENTARY_MAX:
				sprintf(buf1, "MO");
				sprintf(buf2, "%i", keycode - QK_MOMENTARY);
				break;
			case QK_TOGGLE_LAYER ... QK_TOGGLE_LAYER_MAX:
				sprintf(buf1, "TG");
				sprintf(buf2, "%i", keycode - QK_TOGGLE_LAYER);
				break;
			default:
				sprintf(buf1, "??");
				sprintf(buf2, "??");
		}
	// Top-left label
		qp_drawtext_recolor(my_display,
							posx - WIDGET_MATRIX_KEY_WIDTH/2 +3,
							posy - WIDGET_MATRIX_KEY_WIDTH/2 +3,
							WIDGET_MATRIX_LABEL_FONT, buf1, HSV_BLACK, WIDGET_MATRIX_LABEL_BG);
	// Layer number
		qp_drawtext_recolor_center(my_display,
								posx,
								posy + 5, // HOTFIX: +5 for better alignment
								WIDGET_MATRIX_KC_BASIC_FONT,
								buf2, WIDGET_MATRIX_KC_COLOR, WIDGET_MATRIX_KC_BG);
	}