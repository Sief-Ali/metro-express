#include "board.h"

led_t led1;
led_t led_red;
led_t led_green;

btn_t btn1;
btn_t btn2;
btn_t btn3;

void BOARD_Init(void)
{
    led1.pin      = LED_PIN;
    led_red.pin   = LED_RED_PIN;
    led_green.pin = LED_GREEN_PIN;

    btn1.pin    = BTN_PIN;
    btn1.pullup = PULLUP_STATE_ON;

    btn2.pin    = BTN2_PIN;
    btn2.pullup = PULLUP_STATE_ON;

    btn3.pin    = BTN3_PIN;
    btn3.pullup = PULLUP_STATE_ON;
}