#include "led.h"

#include <timelib.h>


void LED_Init(led_t *led)
{
    GPIO_InitPin(led->pin);
}

void LED_On(led_t *led)
{
    GPIO_WritePin(led->pin, GPIO_HIGH);
}

void LED_Off(led_t *led)
{
    GPIO_WritePin(led->pin, GPIO_LOW);
}

void LED_Toggle(led_t *led)
{
    GPIO_TogglePin(led->pin);
}

void LED_Blink(led_t *led, uint16_t blink_delay)
{
  LED_On(led);
  TIMER_Delay(TIMER_0, blink_delay, TIMER_PRESCALER_1024);
  LED_Off(led);
}