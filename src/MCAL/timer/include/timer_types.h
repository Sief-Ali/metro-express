#ifndef TIMER_TYPES_H
#define TIMER_TYPES_H

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

#endif