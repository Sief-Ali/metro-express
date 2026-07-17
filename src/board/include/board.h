#ifndef BOARD_H
#define BOARD_H

#include "btn.h"
#include "led.h"
#include "adc_types.h"

#define DELAY_MS 200

// LEDs
#define LED_READY       GPIO_PIN(C, 2, GPIO_OUT)
#define LED_PROCESSING  GPIO_PIN(C, 3, GPIO_OUT)
#define LED_TICKET      GPIO_PIN(C, 4, GPIO_OUT)
#define LED_ERROR       GPIO_PIN(C, 5, GPIO_OUT)

typedef struct
{
    led_t ready;
    led_t processing;
    led_t ticket;
    led_t error;
} board_leds_t;

extern board_leds_t led;


// Buttons
#define BTN_NEXT        GPIO_PIN(D, 2, GPIO_IN)
#define BTN_CONFIRM     GPIO_PIN(D, 3, GPIO_IN)
#define BTN_CANCEL      GPIO_PIN(B, 2, GPIO_IN)

typedef struct
{
    btn_t next;
    btn_t confirm;
    btn_t cancel;
} board_buttons_t;

extern board_buttons_t btn;

extern adc_config_t pot_adc_config;

#endif