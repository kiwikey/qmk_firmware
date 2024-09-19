SEND_STRING_ENABLE = yes

VPATH += keyboards/kiwikey/0_libraries/graphics

SRC += \
    oled/oled_custom_api.c \
    oled/oled_key_matrix.c \
	oled/oled_ui.c         \
	oled/oled_menu.c       \
	rgb_matrix_name.c \
	via_custom.c      \
    oled.c            \
	encoder.c

# Flash memory saving
SPACE_CADET_ENABLE = no
GRAVE_ESC_ENABLE = no 
MAGIC_ENABLE = no
