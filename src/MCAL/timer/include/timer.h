#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include <stdint.h>
#include "timer_types.h"
#include "timer_registers.h"

void TIMER_ResetControls(const timer_descriptor_t *descriptor);

void TIMER_ConfigureCtcMode(const timer_descriptor_t *descriptor);

void TIMER_SetClock(
  const timer_descriptor_t *descriptor,
  timer_prescaler_t prescaler
);

void TIMER_ClearFlag(const timer_descriptor_t *descriptor, uint8_t flag_bit);

void TIMER_WaitForFlag(const timer_descriptor_t *descriptor, uint8_t flag_bit);

#endif
