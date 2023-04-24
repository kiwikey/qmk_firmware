# This file intentionally left blank

WS2812_DRIVER = vendor

QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS += st7789_spi

VPATH += keyboards/kiwikey/picotest/graphics
SRC += \
    picotest_graphics.c \
	thintel15.qff.c \
	arial30.qff.c \
	pikachu.qgf.c \
	anya01.qgf.c \
	pusheen_240px.qgf.c
