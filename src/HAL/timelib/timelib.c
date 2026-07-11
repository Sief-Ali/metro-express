#include "timelib.h"
#include "timer.h"
#include "timer_registers.h"
#include <stddef.h>

void TIMER_Delay(
  timer_id_t timer,
  uint16_t compare_value,
  timer_prescaler_t prescaler
)
{
  const timer_descriptor_t *descriptor = TIMER_RegistersGet(timer);
  uint8_t compare_flag = 0;

  if ((descriptor == NULL) ||
      (prescaler == TIMER_STOP) ||
      (prescaler >= TIMER_PRESCALER_COUNT) ||
      (compare_value > descriptor->max_count))
  {
      return;
  }

  compare_flag = descriptor->compare_flag_bit[TIMER_COMPARE_A];

  TIMER_ResetControls(descriptor);
  TIMER_Write(&descriptor->counter, 0U);
  TIMER_Write(&descriptor->compare[TIMER_COMPARE_A], compare_value);

  TIMER_ConfigureCtcMode(descriptor);
  TIMER_ClearFlag(descriptor, compare_flag);
  TIMER_SetClock(descriptor, prescaler);

  TIMER_WaitForFlag(descriptor, compare_flag);

  TIMER_ClearFlag(descriptor, compare_flag);
  TIMER_ResetControls(descriptor);
}
