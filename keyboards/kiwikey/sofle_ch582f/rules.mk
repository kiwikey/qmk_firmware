# Real-hardware target: Sofle left half, single unit (not split), output via
# the custom BLUETOOTH_DRIVER bridge to the CH582F over UART0.
UART_DRIVER_REQUIRED = yes
SRC += ch582_bridge.c

QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS += sh1106_i2c
OS_DETECTION_ENABLE = yes