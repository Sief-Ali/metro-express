#include <util/delay.h>
#include <avr/io.h>

// drivers
#include "timelib.h"
#include "timer_types.h"

//components
#include "btn.h"
#include "led.h"
#include "board.h"

int main(void) {
  
  BTN_Init(&btn1);
  
  LED_Init(&led1);

  while (1U) {
    if (BTN_GetState(&btn1) == BTN_PRESSED) {
      LED_Blink(&led1, DELAY_MS);
    } 
    TIMER_Delay(TIMER_0, DELAY_MS, TIMER_PRESCALER_1024);
  }
}