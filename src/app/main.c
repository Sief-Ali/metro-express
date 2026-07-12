#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

// drivers
#include "timer_service.h"
#include "pwm_driver.h"
#include "timer_types.h"

// components
#include "btn.h"
#include "led.h"
#include "board.h"
#include "gpio.h"

/* Matches the SimulIDE schematic (main-v1.sim1):
 *   PD0/PD1/PD2  -> btn1/btn2/btn3   (active-low, internal pull-ups)
 *   PD3          -> led1             (plain GPIO, Led-18)
 *   PD5 (OC1A)   -> Timer1 COMPARE_A (Led-3)
 *   PD4 (OC1B)   -> Timer1 COMPARE_B (Led-15)
 *   PA0          -> led_red          (Led-37)
 *   PA1          -> led_green        (Led-36)
 *
 * Timer usage is split so nothing collides:
 *   TIMER_1 -> PWM cross-fade (its pins are hard-wired to OC1A/OC1B)
 *   TIMER_0 -> heartbeat tick, drives led_green
 *   TIMER_2 -> main loop pacing (TIMER_SERVICE_DelayMs)
 */

/* TIMER_0 is 8-bit: at 16MHz its longest possible period (even at the
 * slowest prescaler) is ~16ms - nowhere near 1 second. So the hardware
 * fires every HEARTBEAT_TICK_MS, and this many ticks are counted in
 * software before the LED actually toggles. This is the standard way to
 * get a long software timer out of a short hardware one. */
#define HEARTBEAT_TICK_MS   10U
#define HEARTBEAT_TICKS     100U /* 100 * 10ms ~= 1s */

static volatile uint16_t heartbeat_ticks = 0U;

static void OnHeartbeatTick(void)
{
    heartbeat_ticks++;
}

int main(void)
{
    pwm_config_t pwm_a =
    {
        .timer = TIMER_1,
        .channel = TIMER_COMPARE_A,
        .mode = PWM_MODE_FAST,
        .frequency_hz = 1000UL
    };

    /* Both channels share Timer1's prescaler/top, so they must use the
     * same frequency - that's a hardware constraint, not a driver one. */
    pwm_config_t pwm_b =
    {
        .timer = TIMER_1,
        .channel = TIMER_COMPARE_B,
        .mode = PWM_MODE_FAST,
        .frequency_hz = 1000UL
    };

    int8_t duty = 0;
    int8_t step = 5;
    uint8_t fade_running = 1U;

    BOARD_Init();

    BTN_Init(&btn1);
    BTN_Init(&btn2);
    BTN_Init(&btn3);

    LED_Init(&led1);
    LED_Init(&led_red);
    LED_Init(&led_green);

    /* PD4/PD5 are driven straight by Timer1's hardware compare output, not
     * through led_t - just need the direction set once. */
    GPIO_InitPin(LED_PWM_A_PIN);
    GPIO_InitPin(LED_PWM_B_PIN);

    PWM_Init(&pwm_a);
    PWM_Init(&pwm_b);

    if (TIMER_SERVICE_SetInterval(TIMER_0, HEARTBEAT_TICK_MS, OnHeartbeatTick) == 0U)
    {
        /* Requested period didn't fit on this timer - flash the red LED
         * fast as a visible "something is wrong" signal instead of
         * silently doing nothing. */
        while (1U)
        {
            LED_Toggle(&led_red);
        }
    }

    sei(); /* global interrupt enable - required for SetInterval to fire */

    while (1U)
    {
        /* Button 1: blocking blink on the plain digital LED */
        if (BTN_GetState(&btn1) == BTN_PRESSED)
        {
            LED_Toggle(&led1);
        }

        /* Button 2: hold to pause the PWM cross-fade */
        fade_running = (BTN_GetState(&btn2) == BTN_PRESSED) ? 0U : 1U;

        /* Button 3: direct on/off control of the red status LED */
        if (BTN_GetState(&btn3) == BTN_PRESSED)
        {
            LED_On(&led_red);
        }
        else
        {
            LED_Off(&led_red);
        }

        if (fade_running != 0U)
        {
            duty += step;

            if ((duty >= 100) || (duty <= 0))
            {
                step = (int8_t)-step;
            }

            PWM_SetDutyCycle(TIMER_1, TIMER_COMPARE_A, (uint8_t)duty);
            PWM_SetDutyCycle(TIMER_1, TIMER_COMPARE_B, (uint8_t)(100 - duty));
        }

        if (heartbeat_ticks >= HEARTBEAT_TICKS)
        {
            heartbeat_ticks = 0U;
            LED_Toggle(&led_green);
        }

        TIMER_SERVICE_DelayMs(TIMER_2, DELAY_MS);
    }
}