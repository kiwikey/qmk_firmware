QUANTUM_PAINTER_DRIVERS += st7789_spi
SERIAL_DRIVER = usart
SEND_STRING_ENABLE = yes

DEFAULT_FOLDER = kiwikey/kiwi5x5/f401

VPATH += keyboards/kiwikey/0.libraries/graphics

SRC += \
    qp_graphics.c \
	qp_custom_api.c \
	qp_includes.c \
	qp_menu.c \
	qp_widget_layer.c \
	qp_widget_matrix.c \
	rgb_matrix_name.c
	
SRC += \
	resources/thintel15.qff.c \
	resources/thintel50.qff.c \
	resources/arial30.qff.c \
	resources/bg_240x240.qgf.c \
	resources/pikachu.qgf.c \
	resources/anya01.qgf.c \
	resources/pusheen_240px.qgf.c \
	resources/icons/lock-caps-ON.qgf.c \
	resources/icons/lock-scrl-ON.qgf.c \
	resources/icons/lock-num-ON.qgf.c \
	resources/icons/lock-caps-OFF.qgf.c \
	resources/icons/lock-scrl-OFF.qgf.c \
	resources/icons/lock-num-OFF.qgf.c