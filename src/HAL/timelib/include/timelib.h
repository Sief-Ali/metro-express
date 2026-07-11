#ifndef TIMELIB_H
#define TIMELIB_H

#include "timer_types.h"
#include <stdint.h>

void TIMER_Delay(
  timer_id_t timer,
  uint16_t compare_value,
  timer_prescaler_t prescaler
);

#endif