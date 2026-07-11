#include "timer.h"

#include <stddef.h>

#include "bit_utils.h"
#include "timer_registers.h"

void TIMER_ResetControls(const timer_descriptor_t *descriptor)
{
    uint8_t index = 0;

    for (index = 0U; index < TIMER_CONTROL_REGISTERS; index++)
    {
        TIMER_Write(&descriptor->control[index], 0U);
    }
}

void TIMER_ConfigureCtcMode(const timer_descriptor_t *descriptor)
{
    uint8_t index = 0;

    for (index = 0U; index < TIMER_CONTROL_REGISTERS; index++)
    {
        TIMER_Update8(&descriptor->control[index],
                      descriptor->waveform_mask[index],
                      descriptor->ctc_value[index]);
    }
}

void TIMER_SetClock(
    const timer_descriptor_t *descriptor,
    timer_prescaler_t prescaler
)
{
    uint8_t clock_value = descriptor->clock_bits[prescaler];

    TIMER_Update8(&descriptor->control[descriptor->clock_control_index],
                  descriptor->clock_mask,
                  (uint8_t)(clock_value << descriptor->clock_shift));
}

void TIMER_ClearFlag(const timer_descriptor_t *descriptor, uint8_t flag_bit)
{
    uint8_t flag_mask = 0U;

    if (flag_bit == TIMER_BIT_UNUSED)
    {
        return;
    }

    SET_BIT(flag_mask, flag_bit);
    TIMER_Write(&descriptor->interrupt_flags, flag_mask);
}

void TIMER_WaitForFlag(const timer_descriptor_t *descriptor, uint8_t flag_bit)
{
    if (flag_bit == TIMER_BIT_UNUSED)
    {
        return;
    }

    while (READ_BIT(TIMER_Read8(&descriptor->interrupt_flags), flag_bit) == 0U)
    {
    }
}