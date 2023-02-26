#include "kawii9v2.h"

#ifdef RGB_MATRIX_ENABLE
led_config_t g_led_config = {
  // Key Matrix to LED Index
  {
    {  4, 5, 6 },
    {  7, 8, 9 },
    { 10,11,12 }
  },
  // LED Index to Physical Position
  {
    /* RGB Underglow */
    {  37,  21 }, {  37,  64 }, { 187,  64 }, { 187,  21 },
    /* RGB Matrix */
    {   0,   0 }, {  74,   0 }, { 149,   0 },
    {   0,  21 }, {  74,  21 }, { 149,  21 },
	{   0,  43 }, {  74,  43 }, { 149,  43 }
  },
  // LED Index to Flag
  {
    /* RGB Underglow */
    2, 2, 2, 2,
    /* RGB Matrix */
	4, 4, 4,
    4, 4, 4,
    4, 4, 4
  }
};
#endif