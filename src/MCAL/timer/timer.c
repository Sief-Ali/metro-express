#include "timer.h"

#include <stddef.h>

#include "bit_utils.h"
#include "timer_registers.h"

static uint8_t TIMER_RegisterIsValid(const timer_register_t *reg)
{
    return ((reg != NULL) &&
            (reg->address != NULL) &&
            (reg->width != TIMER_REGISTER_UNUSED));
}

static uint8_t TIMER_Read8(const timer_register_t *reg)
{
    if (TIMER_RegisterIsValid(reg) == 0U)
    {
        return 0U;
    }

    return *(volatile uint8_t *)reg->address;
}

static void TIMER_Write(const timer_register_t *reg, uint16_t value)
{
    if (TIMER_RegisterIsValid(reg) == 0U)
    {
        return;
    }

    if (reg->width == TIMER_REGISTER_16_BIT)
    {
        *(volatile uint16_t *)reg->address = value;
    }
    else
    {
        *(volatile uint8_t *)reg->address = (uint8_t)value;
    }
}

static void TIMER_Update8(const timer_register_t *reg, uint8_t mask, uint8_t value)
{
    uint8_t current_value = TIMER_Read8(reg);

    current_value &= (uint8_t)~mask;
    current_value |= (uint8_t)(value & mask);

    TIMER_Write(reg, current_value);
}

static void TIMER_ResetControls(const timer_descriptor_t *descriptor)
{
    uint8_t index;

    for (index = 0U; index < TIMER_CONTROL_REGISTERS; index++)
    {
        TIMER_Write(&descriptor->control[index], 0U);
    }
}

static void TIMER_ConfigureCtcMode(const timer_descriptor_t *descriptor)
{
    uint8_t index;

    for (index = 0U; index < TIMER_CONTROL_REGISTERS; index++)
    {
        TIMER_Update8(&descriptor->control[index],
                      descriptor->waveform_mask[index],
                      descriptor->ctc_value[index]);
    }
}

static void TIMER_SetClock(
    const timer_descriptor_t *descriptor,
    timer_prescaler_t prescaler
)
{
    uint8_t clock_value = descriptor->clock_bits[prescaler];

    TIMER_Update8(&descriptor->control[descriptor->clock_control_index],
                  descriptor->clock_mask,
                  (uint8_t)(clock_value << descriptor->clock_shift));
}

static void TIMER_ClearFlag(const timer_descriptor_t *descriptor, uint8_t flag_bit)
{
    uint8_t flag_mask = 0U;

    if (flag_bit == TIMER_BIT_UNUSED)
    {
        return;
    }

    SET_BIT(flag_mask, flag_bit);
    TIMER_Write(&descriptor->interrupt_flags, flag_mask);
}

static void TIMER_WaitForFlag(const timer_descriptor_t *descriptor, uint8_t flag_bit)
{
    if (flag_bit == TIMER_BIT_UNUSED)
    {
        return;
    }

    while (READ_BIT(TIMER_Read8(&descriptor->interrupt_flags), flag_bit) == 0U)
    {
    }
}

void TIMER_Delay(
    timer_id_t timer,
    uint16_t compare_value,
    timer_prescaler_t prescaler
)
{
    const timer_descriptor_t *descriptor = TIMER_RegistersGet(timer);
    uint8_t compare_flag;

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
