#include "logger.h"
#include <util/delay.h>

int  main(void) {

  Logger_Init();

  Logger_Log(
    LOG_INFO,
    "[0x00]:SYS_INIT_OK");
  
  while (1)
  {
    _delay_ms(1000);
  }
}