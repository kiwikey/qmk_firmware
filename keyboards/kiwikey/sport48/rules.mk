QUANTUM_PAINTER_DRIVERS += st7789_spi
SERIAL_DRIVER = usart

DEFAULT_FOLDER = kiwikey/sport48/f401

VPATH += keyboards/kiwikey/0.libraries/graphics
SRC += \
    qp_graphics.c \
	resources/thintel15.qff.c \
	resources/arial30.qff.c \
	resources/pikachu.qgf.c \
	resources/anya01.qgf.c \
	resources/pusheen_240px.qgf.c
