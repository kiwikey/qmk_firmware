// Enable UART0 (GP0=TX, GP1=RX per RP2040's UART0 pin mapping) for the
// bridge link to the CH582F. Disabled by default in GENERIC_RP_RP2040's
// base mcuconf.h — see platforms/chibios/boards/GENERIC_RP_RP2040/configs/mcuconf.h.
#pragma once

#include_next <mcuconf.h>

#undef RP_SIO_USE_UART0
#define RP_SIO_USE_UART0 TRUE
