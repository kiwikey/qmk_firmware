I2C_DRIVER_REQUIRED   = yes
SPI_DRIVER_REQUIRED   = yes
SEND_STRING_ENABLE    = yes
KEYCODE_STRING_ENABLE = yes

QUANTUM_PAINTER_ENABLE = yes
# QUANTUM_PAINTER_DRIVERS += st7789_spi
QUANTUM_PAINTER_DRIVERS += ili9341_spi

# SOURCE FILES
SRC += \
	display/qp_custom_api.c            \
	display/qp_includes.c              \
	display/widgets/qp_widget_matrix.c \
	display/widgets/qp_widget_layer.c  \
	display/widgets/qp_widget_knob.c

# FONTS
SRC += \
	display/resources/thintel15.qff.c    \
	display/resources/roboto20.qff.c     \
	display/resources/roboto25.qff.c     \
	display/resources/robotobold25.qff.c

# ICONS
SRC += \
	display/resources/icons/ico32_brightness.qgf.c  \
	display/resources/icons/ico16_arrow_up.qgf.c    \
	display/resources/icons/ico16_arrow_down.qgf.c  \
	display/resources/icons/ico16_arrow_left.qgf.c  \
	display/resources/icons/ico16_arrow_right.qgf.c
