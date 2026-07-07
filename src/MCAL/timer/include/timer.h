#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include <stdint.h>

typedef enum
{
    TIMER_0,
    TIMER_1,
    TIMER_2,
    TIMER_COUNT
} timer_id_t;

typedef enum
{
    TIMER_STOP = 0,
    TIMER_PRESCALER_1,
    TIMER_PRESCALER_8,
    TIMER_PRESCALER_64,
    TIMER_PRESCALER_256,
    TIMER_PRESCALER_1024,
    TIMER_PRESCALER_COUNT
} timer_prescaler_t;

void TIMER_Delay(
  timer_id_t timer,
  uint16_t compare_value,
  timer_prescaler_t prescaler
);

#endif
