#include "btn.h"
#include "gpio.h"

void BTN_Init(btn_t *btn)
{
    GPIO_InitPin_WithPullup(
        btn->pin,
        btn->pullup
    );
}

btn_state_t BTN_GetState(btn_t *btn)
{
  gpio_state_t state =
    GPIO_ReadPin(btn->pin);

  switch (btn->pullup) {
    case PULLUP_STATE_ON:
       return (state == GPIO_LOW)
          ? BTN_PRESSED
          : BTN_RELEASED;
    default:
      return (state == GPIO_HIGH)
          ? BTN_PRESSED
          : BTN_RELEASED;
  }
}