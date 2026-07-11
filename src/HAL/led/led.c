#include "led.h"

#include <timer_service.h>


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
  for (int x=0; x < 5; x++) {
    TIMER_SERVICE_DelayMs(TIMER_1, blink_delay);
  }
  LED_Off(led);
}