#ifndef TIMER_REGISTERS_H
#define TIMER_REGISTERS_H

#include <stdint.h>

#include "timer.h"

#define TIMER_COMPARE_CHANNELS 2U
#define TIMER_CONTROL_REGISTERS 2U
#define TIMER_BIT_UNUSED 0xFFU

typedef enum
{
    TIMER_REGISTER_UNUSED = 0,
    TIMER_REGISTER_8_BIT,
    TIMER_REGISTER_16_BIT
} timer_register_width_t;

typedef enum
{
    TIMER_COMPARE_A = 0,
    TIMER_COMPARE_B,
    TIMER_COMPARE_COUNT
} timer_compare_channel_t;

typedef enum
{
    TIMER_CAP_COMPARE_A     = (1U << 0),
    TIMER_CAP_COMPARE_B     = (1U << 1),
    TIMER_CAP_INPUT_CAPTURE = (1U << 2),
    TIMER_CAP_ASYNC         = (1U << 3)
} timer_capability_t;

typedef struct
{
    volatile void *address;
    timer_register_width_t width;
} timer_register_t;

typedef struct
{
    timer_register_t control[TIMER_CONTROL_REGISTERS];
    timer_register_t counter;
    timer_register_t compare[TIMER_COMPARE_CHANNELS];
    timer_register_t interrupt_mask;
    timer_register_t interrupt_flags;

    uint8_t waveform_mask[TIMER_CONTROL_REGISTERS];
    uint8_t ctc_value[TIMER_CONTROL_REGISTERS];

    uint8_t clock_control_index;
    uint8_t clock_mask;
    uint8_t clock_shift;
    uint8_t clock_bits[TIMER_PRESCALER_COUNT];

    uint8_t compare_interrupt_bit[TIMER_COMPARE_CHANNELS];
    uint8_t compare_flag_bit[TIMER_COMPARE_CHANNELS];
    uint8_t overflow_interrupt_bit;
    uint8_t overflow_flag_bit;
    uint8_t input_capture_interrupt_bit;
    uint8_t input_capture_flag_bit;

    uint16_t max_count;
    uint8_t capabilities;
} timer_descriptor_t;

const timer_descriptor_t *TIMER_RegistersGet(timer_id_t timer);

#endif
