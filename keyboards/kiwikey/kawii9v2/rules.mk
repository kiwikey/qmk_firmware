SEND_STRING_ENABLE = yes

VPATH += keyboards/kiwikey/0_libraries/custom_display

SRC += \
    oled.c            \
	encoder.c         \
	via_custom.c      \
	rgb_matrix_name.c \
    oled/oled_custom_api.c \
	oled/oled_ui.c         \
	oled/oled_menu.c       \
	oled/oled_key_matrix.c


# AVR firmware size reducing
SPACE_CADET_ENABLE = no
GRAVE_ESC_ENABLE = no 
MAGIC_ENABLE = no
