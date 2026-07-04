#include "timer.h"
#include "timer_registers.h"
#include "timer_private.h"

#include "bit_utils.h"

static uint8_t TIMER_PrescalerToCS(timer_id_t timer, timer_prescaler_t prescaler)
{
    if (timer == TIMER_2)
    {
        switch (prescaler)
        {
            case TIMER_PRESCALER_1:
                return 1U;

            case TIMER_PRESCALER_8:
                return 2U;

            case TIMER_PRESCALER_64:
                return 4U;

            case TIMER_PRESCALER_256:
                return 6U;

            case TIMER_PRESCALER_1024:
                return 7U;

            case TIMER_STOP:
            default:
                return 0U;
        }
    }

    switch (prescaler)
    {
        case TIMER_PRESCALER_1:
            return 1U;

        case TIMER_PRESCALER_8:
            return 2U;

        case TIMER_PRESCALER_64:
            return 3U;

        case TIMER_PRESCALER_256:
            return 4U;

        case TIMER_PRESCALER_1024:
            return 5U;

        case TIMER_STOP:
        default:
            return 0U;
    }
}

static void TIMER_ClearFlag(uint8_t flag)
{
    TIMER_TIFR = (uint8_t)(1U << flag);
}

static void TIMER_Delay0(uint8_t compare_value, timer_prescaler_t prescaler)
{
    TIMER_TCCR0 = 0U;
    TIMER_TCNT0 = 0U;
    TIMER_OCR0 = compare_value;

    SET_BIT(TIMER_TCCR0, TIMER_WGM01);

    TIMER_ClearFlag(TIMER_OCF0);
    TIMER_TCCR0 |= TIMER_PrescalerToCS(TIMER_0, prescaler);

    while (READ_BIT(TIMER_TIFR, TIMER_OCF0) == 0U)
    {
    }

    TIMER_ClearFlag(TIMER_OCF0);

    TIMER_TCCR0 = 0U;
}

static void TIMER_Delay1(uint16_t compare_value, timer_prescaler_t prescaler)
{
    TIMER_TCCR1A = 0U;
    TIMER_TCCR1B = 0U;
    TIMER_TCNT1 = 0U;
    TIMER_OCR1A = compare_value;

    SET_BIT(TIMER_TCCR1B, TIMER_WGM12);

    TIMER_ClearFlag(TIMER_OCF1A);
    TIMER_TCCR1B |= TIMER_PrescalerToCS(TIMER_1, prescaler);

    while (READ_BIT(TIMER_TIFR, TIMER_OCF1A) == 0U)
    {
    }

    TIMER_ClearFlag(TIMER_OCF1A);

    TIMER_TCCR1B = 0U;
}

static void TIMER_Delay2(uint8_t compare_value, timer_prescaler_t prescaler)
{
    TIMER_TCCR2 = 0U;
    TIMER_TCNT2 = 0U;
    TIMER_OCR2 = compare_value;

    SET_BIT(TIMER_TCCR2, TIMER_WGM21);

    TIMER_ClearFlag(TIMER_OCF2);
    TIMER_TCCR2 |= TIMER_PrescalerToCS(TIMER_2, prescaler);

    while (READ_BIT(TIMER_TIFR, TIMER_OCF2) == 0U)
    {
    }

    TIMER_ClearFlag(TIMER_OCF2);

    TIMER_TCCR2 = 0U;
}

void TIMER_Delay(
    timer_id_t timer,
    uint16_t compare_value,
    timer_prescaler_t prescaler
)
{
    if (prescaler == TIMER_STOP)
    {
        return;
    }

    switch (timer)
    {
        case TIMER_0:
            TIMER_Delay0((uint8_t)compare_value, prescaler);
            break;

        case TIMER_1:
            TIMER_Delay1(compare_value, prescaler);
            break;

        case TIMER_2:
            TIMER_Delay2((uint8_t)compare_value, prescaler);
            break;

        default:
            break;
    }
}
