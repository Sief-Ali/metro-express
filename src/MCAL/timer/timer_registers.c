#include "timer_registers.h"

#include <avr/io.h>
#include <stddef.h>

#define TIMER_REG8(REG)  { (volatile void *)&(REG), TIMER_REGISTER_8_BIT }
#define TIMER_REG16(REG) { (volatile void *)&(REG), TIMER_REGISTER_16_BIT }
#define TIMER_NO_REG     { NULL, TIMER_REGISTER_UNUSED }

#define TIMER_NO_BIT TIMER_BIT_UNUSED

uint8_t TIMER_RegisterIsValid(const timer_register_t *reg)
{
    return ((reg != NULL) &&
            (reg->address != NULL) &&
            (reg->width != TIMER_REGISTER_UNUSED));
}

uint8_t TIMER_Read8(const timer_register_t *reg)
{
    if (TIMER_RegisterIsValid(reg) == 0U)
    {
        return 0U;
    }

    return *(volatile uint8_t *)reg->address;
}

void TIMER_Write(const timer_register_t *reg, uint16_t value)
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

void TIMER_Update8(const timer_register_t *reg, uint8_t mask, uint8_t value)
{
    uint8_t current_value = TIMER_Read8(reg);

    current_value &= (uint8_t)~mask;
    current_value |= (uint8_t)(value & mask);

    TIMER_Write(reg, current_value);
}

static const timer_descriptor_t timer_descriptors[TIMER_COUNT] =
{
    [TIMER_0] =
    {
        .control = { TIMER_REG8(TCCR0), TIMER_NO_REG },
        .counter = TIMER_REG8(TCNT0),
        .compare = { TIMER_REG8(OCR0), TIMER_NO_REG },
        .interrupt_mask = TIMER_REG8(TIMSK),
        .interrupt_flags = TIMER_REG8(TIFR),

        .waveform_mask = { (uint8_t)((1U << WGM00) | (1U << WGM01)), 0U },
        .ctc_value = { (uint8_t)(1U << WGM01), 0U },

        .clock_control_index = 0U,
        .clock_mask = (uint8_t)((1U << CS02) | (1U << CS01) | (1U << CS00)),
        .clock_shift = CS00,
        .clock_bits =
        {
            [TIMER_STOP] = 0U,
            [TIMER_PRESCALER_1] = 1U,
            [TIMER_PRESCALER_8] = 2U,
            [TIMER_PRESCALER_64] = 3U,
            [TIMER_PRESCALER_256] = 4U,
            [TIMER_PRESCALER_1024] = 5U
        },

        .compare_interrupt_bit = { OCIE0, TIMER_NO_BIT },
        .compare_flag_bit = { OCF0, TIMER_NO_BIT },
        .overflow_interrupt_bit = TOIE0,
        .overflow_flag_bit = TOV0,
        .input_capture_interrupt_bit = TIMER_NO_BIT,
        .input_capture_flag_bit = TIMER_NO_BIT,

        .max_count = UINT8_MAX,
        .capabilities = TIMER_CAP_COMPARE_A
    },

    [TIMER_1] =
    {
        .control = { TIMER_REG8(TCCR1A), TIMER_REG8(TCCR1B) },
        .counter = TIMER_REG16(TCNT1),
        .compare = { TIMER_REG16(OCR1A), TIMER_REG16(OCR1B) },
        .interrupt_mask = TIMER_REG8(TIMSK),
        .interrupt_flags = TIMER_REG8(TIFR),

        .waveform_mask =
        {
            (uint8_t)((1U << WGM11) | (1U << WGM10)),
            (uint8_t)((1U << WGM13) | (1U << WGM12))
        },
        .ctc_value = { 0U, (uint8_t)(1U << WGM12) },

        .clock_control_index = 1U,
        .clock_mask = (uint8_t)((1U << CS12) | (1U << CS11) | (1U << CS10)),
        .clock_shift = CS10,
        .clock_bits =
        {
            [TIMER_STOP] = 0U,
            [TIMER_PRESCALER_1] = 1U,
            [TIMER_PRESCALER_8] = 2U,
            [TIMER_PRESCALER_64] = 3U,
            [TIMER_PRESCALER_256] = 4U,
            [TIMER_PRESCALER_1024] = 5U
        },

        .compare_interrupt_bit = { OCIE1A, OCIE1B },
        .compare_flag_bit = { OCF1A, OCF1B },
        .overflow_interrupt_bit = TOIE1,
        .overflow_flag_bit = TOV1,
        .input_capture_interrupt_bit = TICIE1,
        .input_capture_flag_bit = ICF1,

        .max_count = UINT16_MAX,
        .capabilities =
            (uint8_t)(TIMER_CAP_COMPARE_A |
                      TIMER_CAP_COMPARE_B |
                      TIMER_CAP_INPUT_CAPTURE)
    },

    [TIMER_2] =
    {
        .control = { TIMER_REG8(TCCR2), TIMER_NO_REG },
        .counter = TIMER_REG8(TCNT2),
        .compare = { TIMER_REG8(OCR2), TIMER_NO_REG },
        .interrupt_mask = TIMER_REG8(TIMSK),
        .interrupt_flags = TIMER_REG8(TIFR),

        .waveform_mask = { (uint8_t)((1U << WGM20) | (1U << WGM21)), 0U },
        .ctc_value = { (uint8_t)(1U << WGM21), 0U },

        .clock_control_index = 0U,
        .clock_mask = (uint8_t)((1U << CS22) | (1U << CS21) | (1U << CS20)),
        .clock_shift = CS20,
        .clock_bits =
        {
            [TIMER_STOP] = 0U,
            [TIMER_PRESCALER_1] = 1U,
            [TIMER_PRESCALER_8] = 2U,
            [TIMER_PRESCALER_64] = 4U,
            [TIMER_PRESCALER_256] = 6U,
            [TIMER_PRESCALER_1024] = 7U
        },

        .compare_interrupt_bit = { OCIE2, TIMER_NO_BIT },
        .compare_flag_bit = { OCF2, TIMER_NO_BIT },
        .overflow_interrupt_bit = TOIE2,
        .overflow_flag_bit = TOV2,
        .input_capture_interrupt_bit = TIMER_NO_BIT,
        .input_capture_flag_bit = TIMER_NO_BIT,

        .max_count = UINT8_MAX,
        .capabilities = (uint8_t)(TIMER_CAP_COMPARE_A | TIMER_CAP_ASYNC)
    }
};

const timer_descriptor_t *TIMER_RegistersGet(timer_id_t timer)
{
    if (timer >= TIMER_COUNT)
    {
        return NULL;
    }

    return &timer_descriptors[timer];
}
