#ifndef BOARD_H
#define BOARD_H

#include "btn.h"
#include "led.h"

#define DELAY_MS 200

// LEDs
#define LED_PIN     GPIO_PIN(D, 3, GPIO_OUT)
extern const led_t led1;

// Buttons
#define BTN_PIN     GPIO_PIN(D, 0, GPIO_IN)
extern const btn_t btn1;


#endif