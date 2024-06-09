SPACE_CADET_ENABLE = no
GRAVE_ESC_ENABLE = no 
MAGIC_ENABLE = no

VPATH += keyboards/kiwikey/kawii9v2/lib

SRC += \
    oled_custom_api.c \
    rgb_matrix_name.c \
    oled_key_matrix.c \
	oled_wpm_graph.c  \
    oled_ui.c         \
	oled_menu.c       \
	via_custom.c      \
    oled.c
