#pragma once

#define HAL_USE_SPI TRUE
//#define HAL_USE_I2C TRUE
#define HAL_USE_PWM TRUE
//#define HAL_USE_PAL TRUE
// #define HAL_USE_GPT TRUE

#define HAL_USE_SERIAL TRUE
#define SERIAL_USB_BUFFERS_SIZE 256

#include_next <halconf.h>
