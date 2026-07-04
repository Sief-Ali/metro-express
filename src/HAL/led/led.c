#include "led.h"

#include <util/delay.h>


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
  while (blink_delay--) {
    _delay_ms(1);
  }
  LED_Off(led);
}