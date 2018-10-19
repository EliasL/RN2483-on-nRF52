#include "Simple_debug_printf.h"
#include <string.h>
#include <stdint.h>

void debug_print(const char * sFormat, ...) {
  va_list ParamList;
  va_start(ParamList, sFormat);
  SEGGER_RTT_vprintf(0, sFormat, &ParamList);
  va_end(ParamList);
  SEGGER_RTT_printf(0, "\r\n");
}
