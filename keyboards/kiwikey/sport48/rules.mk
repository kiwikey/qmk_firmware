QUANTUM_PAINTER_DRIVERS += st7789_spi
SERIAL_DRIVER = usart

# DEFAULT_FOLDER = kiwikey/sport48/f401

VPATH += keyboards/kiwikey/0_libraries/custom_display
SRC += \
    qp_graphics.c \
	qp/resources/thintel15.qff.c \
	qp/resources/pikachu.qgf.c \
	qp/resources/anya01.qgf.c \
	qp/resources/pusheen_240px.qgf.c
