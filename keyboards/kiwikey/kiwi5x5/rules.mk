QUANTUM_PAINTER_DRIVERS += st7789_spi
SERIAL_DRIVER = usart

DEFAULT_FOLDER = kiwikey/kiwi5x5/f401

VPATH += keyboards/kiwikey/0.libraries/graphics
SRC += \
    qp_graphics.c \
	qp_matrix_reactive.c \
	resources/thintel15.qff.c \
	resources/thintel40.qff.c \
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