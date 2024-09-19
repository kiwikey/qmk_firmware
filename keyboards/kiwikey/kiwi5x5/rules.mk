QUANTUM_PAINTER_DRIVERS += st7789_spi
SEND_STRING_ENABLE = yes

DEFAULT_FOLDER = kiwikey/kiwi5x5/f401

VPATH += keyboards/kiwikey/0_libraries/custom_display

SRC += \
	rgb_matrix_name.c \
	via_custom.c      \
    qp_graphics.c     \
	qp/qp_custom_api.c     \
	qp/qp_includes.c       \
	qp/qp_menu.c           \
	qp/qp_widget_layer.c   \
	qp/qp_widget_matrix.c  \
	qp/qp_widget_rgbstat.c

	
SRC += \
	qp/resources/thintel15.qff.c    \
	qp/resources/roboto20.qff.c     \
	qp/resources/roboto25.qff.c     \
	qp/resources/robotobold25.qff.c
	
SRC += \
	qp/resources/pikachu.qgf.c       \
	qp/resources/anya01.qgf.c        \
	qp/resources/pusheen_240px.qgf.c \
	qp/resources/gif_nyan120px.qgf.c \
	qp/resources/gif_cat01.qgf.c     \
	qp/resources/gif_cat02.qgf.c     \
	qp/resources/gif_dog01.qgf.c
	
SRC += \
	qp/resources/icons/ico32_brightness.qgf.c
	# qp/resources/icons/lock-caps-ON.qgf.c \
	# qp/resources/icons/lock-scrl-ON.qgf.c \
	# qp/resources/icons/lock-num-ON.qgf.c \
	# qp/resources/icons/lock-caps-OFF.qgf.c \
	# qp/resources/icons/lock-scrl-OFF.qgf.c \
	# qp/resources/icons/lock-num-OFF.qgf.c \