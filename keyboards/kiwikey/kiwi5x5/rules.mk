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
	qp_widget_rgbstat.c \
	rgb_matrix_name.c \
	via_custom.c
	
SRC += \
	resources/thintel15.qff.c \
	resources/roboto20.qff.c \
	resources/roboto25.qff.c \
	resources/robotobold25.qff.c
	
SRC += \
	resources/pikachu.qgf.c \
	resources/anya01.qgf.c \
	resources/pusheen_240px.qgf.c \
	resources/gif_nyan120px.qgf.c \
	resources/gif_cat01.qgf.c \
	resources/gif_cat02.qgf.c \
	resources/gif_dog01.qgf.c
	
SRC += \
	resources/icons/ico32_brightness.qgf.c

	# resources/icons/lock-caps-ON.qgf.c \
	# resources/icons/lock-scrl-ON.qgf.c \
	# resources/icons/lock-num-ON.qgf.c \
	# resources/icons/lock-caps-OFF.qgf.c \
	# resources/icons/lock-scrl-OFF.qgf.c \
	# resources/icons/lock-num-OFF.qgf.c \