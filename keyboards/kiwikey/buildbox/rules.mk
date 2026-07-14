I2C_DRIVER_REQUIRED   = yes
SPI_DRIVER_REQUIRED   = yes
SEND_STRING_ENABLE    = yes
KEYCODE_STRING_ENABLE = yes
DEFERRED_EXEC_ENABLE  = yes

# QUANTUM PAINTER
QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS += st7789_spi
# QUANTUM_PAINTER_DRIVERS += ili9341_spi

# CUSTOM MATRIX
CUSTOM_MATRIX = lite
SRC += matrix.c

# OTHERS
QUANTUM_LIB_SRC += i2c_master.c
SRC += \
	features/via_custom.c              \
	sensor/sensors_handler.c           \
	sensor/as5600.c

# QP SOURCE FILES
SRC += \
	display/qp_graphics.c              \
	display/qp_custom_api.c            \
	display/qp_includes.c              \
	display/widgets/qp_menu.c          \
	display/widgets/qp_widget_matrix.c \
	display/widgets/qp_widget_layer.c  \
	display/widgets/qp_widget_knob.c   \
	display/widgets/qp_widget_breakout.c

# FONTS
SRC += \
	display/resources/fonts/thintel15.qff.c         \
	display/resources/fonts/roboto20.qff.c          \
	display/resources/fonts/roboto25.qff.c          \
	display/resources/fonts/robotobold25.qff.c      \
	display/resources/fonts/font_oled.qff.c         \
	display/resources/fonts/font_proggy_clean.qff.c \
	display/resources/fonts/font_proggy_tiny.qff.c

# ICONS
SRC += \
	display/resources/icons/ico32_brightness.qgf.c  \
	display/resources/icons/ico16_arrow_up.qgf.c    \
	display/resources/icons/ico16_arrow_down.qgf.c  \
	display/resources/icons/ico16_arrow_left.qgf.c  \
	display/resources/icons/ico16_arrow_right.qgf.c \
	display/resources/icons/ico16_layer.qgf.c       \
	display/resources/icons/ico32_menu.qgf.c

# IMAGES & ANIMATIONS
SRC += \
	display/resources/graphics/gif_bootup01.qgf.c   \
	display/resources/graphics/gif_cat01.qgf.c      \
	display/resources/graphics/anya01.qgf.c         \
	display/resources/graphics/gif_nyan120px.qgf.c