#include <util/delay.h>

//components
#include "btn.h"
#include "led.h"
#include "board.h"

int main(void) {
  
  BTN_Init(&btn1);
  
  LED_Init(&led1);

  while (1U) {
    if (BTN_GetState(&btn1) == BTN_PRESSED) {
      LED_Blink(&led1, BLINK_DELAY);
    } 
    _delay_ms(DELAY_MS);
  }
}