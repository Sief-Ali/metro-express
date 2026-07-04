#ifndef BTN_H
#define BTN_H
#include "gpio.h"

typedef struct
{
    gpio_pin_t pin;
    pullup_state_t pullup;
} btn_t;

typedef enum 
{
    BTN_PRESSED = 1U,
    BTN_RELEASED = 0U
} btn_state_t;

void BTN_Init(btn_t *btn);
btn_state_t BTN_GetState(btn_t *btn);

#endif