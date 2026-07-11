#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H

#include <stdint.h>

#include "timer_types.h"

/* -------------------------------------------------------------------------- */
/* Timer Service                                                               */
/*                                                                              */
/* HAL-layer wrapper around the MCAL timer driver's public API only (timer.h
 * timer_types.h). This file never touches registers, ISR vector names, or
 * anything AVR-specific - all of that stays inside MCAL/timer.
 */
/* -------------------------------------------------------------------------- */

/* Blocking delay. Busy-waits for `milliseconds` on `timer`, then leaves it
 * stopped. `timer` is dedicated for the duration of the call - don't use a
 * timer that's already running a PWM signal or a SetInterval callback. */
void TIMER_SERVICE_DelayMs(
    timer_id_t timer,
    uint16_t milliseconds);

/* Calls `callback` once, `milliseconds` from now, then automatically stops
 * and detaches itself. Non-blocking. */
void TIMER_SERVICE_SetTimeout(
    timer_id_t timer,
    uint16_t milliseconds,
    timer_callback_t callback);

/* Calls `callback` every `milliseconds`, repeatedly, until
 * TIMER_SERVICE_Cancel is called. Non-blocking. */
void TIMER_SERVICE_SetInterval(
    timer_id_t timer,
    uint16_t milliseconds,
    timer_callback_t callback);

/* Stops `timer` and detaches any timeout/interval callback registered on
 * it via this service. Safe to call even if nothing is armed. */
void TIMER_SERVICE_Cancel(timer_id_t timer);

#endif /* TIMER_SERVICE_H */
