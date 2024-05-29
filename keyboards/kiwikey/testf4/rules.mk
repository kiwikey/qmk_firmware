QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS += st7789_spi

VPATH += keyboards/kiwikey/testf4/graphics
SRC += \
	pikachu.qgf.c \
	pusheen_240px.qgf.c

DEFAULT_FOLDER = kiwikey/testf4/f401
