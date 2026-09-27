I2C_DRIVER_REQUIRED   = yes
SPI_DRIVER_REQUIRED   = yes
SEND_STRING_ENABLE    = yes
KEYCODE_STRING_ENABLE = yes
DEFERRED_EXEC_ENABLE  = yes
COMBO_ENABLE          = yes

INTROSPECTION_KEYMAP_C = features/combos.c

# QUANTUM PAINTER - choose only 1 driver to enable
QUANTUM_PAINTER_ENABLE = yes
# QUANTUM_PAINTER_DRIVERS += ili9341_spi
QUANTUM_PAINTER_DRIVERS += st7789_spi

# CUSTOM MATRIX
CUSTOM_MATRIX = lite
SRC += matrix.c

# OTHERS
QUANTUM_LIB_SRC += i2c_master.c
SRC += \
	features/via_custom.c               \
	features/eeprom_custom.c            \
	features/knob_custom.c              \
	features/webhid_stream.c            \
	features/webhid_shadow.c            \
	sensor/sensors_handler.c            \
	sensor/as5600.c

# QP SOURCE FILES
SRC += \
	display/qp_graphics.c               \
	display/qp_custom_api.c             \
	display/qp_includes.c               \
	display/widgets/qp_menu.c           \
	display/widgets/qp_widget_matrix.c  \
	display/widgets/qp_widget_layer.c   \
	display/widgets/qp_widget_knob.c    \
	display/widgets/qp_widget_status.c  \
	display/widgets/qp_widget_breakout.c\
	display/widgets/qp_widget_screensaver.c\
	display/widgets/screensaver_matrix_rain.c\
	display/widgets/screensaver_sleeping_zzz.c\
	display/widgets/screensaver_starry_night.c\
	display/widgets/tutorial.c

# FONTS
SRC += \
	display/resources/fonts/thintel16.qff.c         \
	display/resources/fonts/thintel32.qff.c         \
	display/resources/fonts/font_oled.qff.c         \
	display/resources/fonts/nanoplex16.qff.c        \
	display/resources/fonts/nanoplex32.qff.c        \
	display/resources/fonts/font16.qff.c

# ICONS
SRC += \
	display/resources/icons/ico32_brightness.qgf.c  \
	display/resources/icons/ico16_arrow_up.qgf.c    \
	display/resources/icons/ico16_arrow_down.qgf.c  \
	display/resources/icons/ico16_arrow_left.qgf.c  \
	display/resources/icons/ico16_arrow_right.qgf.c \
	display/resources/icons/ico12_arrow_left.qgf.c  \
	display/resources/icons/ico12_arrow_right.qgf.c \
	display/resources/icons/ico22_gear.qgf.c        \
	display/resources/graphics/ico18_heart.qgf.c

# ICONS - 22x22 batch (display/resources/icons/22x22/)
SRC += \
	display/resources/icons/22x22/25.qgf.c               \
	display/resources/icons/22x22/47.qgf.c               \
	display/resources/icons/22x22/68.qgf.c               \
	display/resources/icons/22x22/72.qgf.c               \
	display/resources/icons/22x22/74.qgf.c               \
	display/resources/icons/22x22/86.qgf.c               \
	display/resources/icons/22x22/88.qgf.c               \
	display/resources/icons/22x22/Application.qgf.c      \
	display/resources/icons/22x22/Boss.qgf.c             \
	display/resources/icons/22x22/Calculator.qgf.c       \
	display/resources/icons/22x22/color_wheel.qgf.c      \
	display/resources/icons/22x22/Component.qgf.c        \
	display/resources/icons/22x22/Desktop.qgf.c          \
	display/resources/icons/22x22/Earth.qgf.c            \
	display/resources/icons/22x22/Favourites.qgf.c       \
	display/resources/icons/22x22/Film.qgf.c             \
	display/resources/icons/22x22/game_controller.qgf.c  \
	display/resources/icons/22x22/Gear.qgf.c             \
	display/resources/icons/22x22/Globe.qgf.c            \
	display/resources/icons/22x22/Heart.qgf.c            \
	display/resources/icons/22x22/Heart1.qgf.c           \
	display/resources/icons/22x22/Mail.qgf.c             \
	display/resources/icons/22x22/Mouse.qgf.c            \
	display/resources/icons/22x22/Online.qgf.c

# IMAGES & ANIMATIONS
SRC += \
	display/resources/graphics/gif_bootup01.qgf.c   \
	display/resources/graphics/gif_cat01.qgf.c      \
	display/resources/graphics/anya01.qgf.c         \
	display/resources/graphics/gif_nyan120px.qgf.c  \
	display/resources/graphics/knob.qgf.c