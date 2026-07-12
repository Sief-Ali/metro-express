#include <avr/io.h>
#include <stdint.h>

// drivers
#include "timer_service.h"
#include "pwm_driver.h"
#include "timer_types.h"
#include "ext_int.h"

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
 *
 * btn3 (PD2) is the only button that can use a real hardware interrupt on
 * this chip - INT0 is fixed to PD2 by the ATmega32's silicon. INT1 (PD3)
 * is already led1's pin, and INT2 (PB2) isn't wired to anything in this
 * schematic, so btn1/btn2 stay on polling (BTN_GetState) as before.
 *
 * KNOWN ISSUE (workaround below): LED_Toggle appears to reconfigure TIMER_1
 * internally (most likely a delay routine hardcoded to TIMER_1, left over
 * from before PWM existed in this project). That knocks TIMER_1 out of
 * Fast PWM mode and stops it. Until led.c is fixed to leave TIMER_1 alone,
 * PWM is re-armed after every button-1 press as a workaround.
 */

#define HEARTBEAT_TICK_MS   10U
#define HEARTBEAT_TICKS     100U /* 100 * 10ms ~= 1s */

static volatile uint16_t heartbeat_ticks = 0U;
static volatile uint8_t red_led_toggle_flag = 0U;

static void OnHeartbeatTick(void)
{
    heartbeat_ticks++;
}

/* Runs inside ext_int's ISR (INT0_vect, via EXT_INT_RegisterCallback).
 * Keep it tiny - just flag the edge, do the real work in the main loop. */
static void OnBtn3Edge(void)
{
    red_led_toggle_flag = 1U;
}

static pwm_config_t pwm_a =
{
    .timer = TIMER_1,
    .channel = TIMER_COMPARE_A,
    .mode = PWM_MODE_FAST,
    .frequency_hz = 1000UL
};

/* Both channels share Timer1's prescaler/top, so they must use the same
 * frequency - that's a hardware constraint, not a driver one. */
static pwm_config_t pwm_b =
{
    .timer = TIMER_1,
    .channel = TIMER_COMPARE_B,
    .mode = PWM_MODE_FAST,
    .frequency_hz = 1000UL
};

/* WORKAROUND: re-applies Timer1's PWM mode/clock and restores the fade's
 * current duty cycle. Call this after anything that might have
 * reconfigured TIMER_1 out from under the PWM (currently: LED_Toggle). */
static void RestorePwm(int8_t duty)
{
    PWM_Init(&pwm_a);
    PWM_Init(&pwm_b);
    PWM_SetDutyCycle(TIMER_1, TIMER_COMPARE_A, (uint8_t)duty);
    PWM_SetDutyCycle(TIMER_1, TIMER_COMPARE_B, (uint8_t)(100 - duty));
}

int main(void)
{
    int8_t duty = 0;
    int8_t step = 5;
    uint8_t fade_running = 1U;

    BOARD_Init();

    BTN_Init(&btn1);
    BTN_Init(&btn2);
    /* BTN_Init(&btn3) still runs - it configures PD2's direction/pull-up
     * via gpio.h, which ext_int deliberately never touches itself. */
     BTN_Init(&btn3);

     BTN_InitInterrupt(
         &btn3,
         EXT_INT_TRIGGER_FALLING_EDGE,
         OnBtn3Edge); 

    LED_Init(&led1);
    LED_Init(&led_red);
    LED_Init(&led_green);

    /* PD4/PD5 are driven straight by Timer1's hardware compare output, not
     * through led_t - just need the direction set once. */
    GPIO_InitPin(LED_PWM_A_PIN);
    GPIO_InitPin(LED_PWM_B_PIN);

    RestorePwm(duty);

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

    EXT_INT_GlobalEnable(); /* required for both TIMER_0's ISR and INT0's ISR to fire */

    while (1U)
    {
        /* Button 1: blocking blink on the plain digital LED.
         * WORKAROUND: LED_Toggle appears to disturb TIMER_1, so PWM is
         * re-armed immediately afterward - see RestorePwm's comment. */
        if (BTN_GetState(&btn1) == BTN_PRESSED)
        {
            LED_Toggle(&led1);
            RestorePwm(duty);
        }

        /* Button 2: hold to pause the PWM cross-fade */
        fade_running = (BTN_GetState(&btn2) == BTN_PRESSED) ? 0U : 1U;

        /* Button 3: now interrupt-driven (INT0) instead of polled. This
         * naturally becomes toggle-on-press rather than the old
         * on-while-held behavior - that's the idiomatic way to react to
         * an edge rather than a level. */
        if (red_led_toggle_flag != 0U)
        {
            red_led_toggle_flag = 0U;
            LED_Toggle(&led_red);
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