#include <util/delay.h>

//drivers
#include "gpio.h"

//components
#include "btn.h"
#include "led.h"

#define DELAY 200

//board
#define BTN_PIN GPIO_PIN(D, 0, GPIO_IN)

#define LED_PIN GPIO_PIN(D, 3, GPIO_OUT)

int main(void) {
  btn_t btn1 =
  {
      .pin = BTN_PIN,
      .pullup = PULLUP_STATE_ON
  }; 
  led_t led1 = 
  {
    .pin= LED_PIN,
  };
  
  BTN_Init(&btn1);
  
  LED_Init(&led1);

  while (1U) {
    if (BTN_GetState(&btn1) == BTN_PRESSED) {
      LED_Blink(&led1, BLINK_DELAY);
    } 
    _delay_ms(DELAY);
  }
}