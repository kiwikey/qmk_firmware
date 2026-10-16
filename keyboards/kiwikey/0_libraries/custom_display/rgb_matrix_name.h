#ifndef RGB_MATRIX_NAME
#define RGB_MATRIX_NAME

#include "quantum.h"

const char* rgb_matrix_name(uint8_t effect); // returns a string in flash (PROGMEM), print with oled_write_*_P()

#endif /* RGB_MATRIX_NAME */
