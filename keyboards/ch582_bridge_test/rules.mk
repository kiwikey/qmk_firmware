# Minimal bring-up target: proves the custom BLUETOOTH_DRIVER + UART link
# to the CH582F bridge firmware works, before any real keyboard is built.
UART_DRIVER_REQUIRED = yes
SRC += ch582_bridge.c
