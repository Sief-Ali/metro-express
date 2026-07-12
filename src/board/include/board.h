#ifndef BOARD_H
#define BOARD_H

#include "btn.h"
#include "led.h"

#define DELAY_MS 200

// LEDs
#define LED_PIN         GPIO_PIN(D, 3, GPIO_OUT)   // Led-18, plain digital
#define LED_PWM_A_PIN   GPIO_PIN(D, 5, GPIO_OUT)   // Led-3,  OC1A (Timer1 COMPARE_A)
#define LED_PWM_B_PIN   GPIO_PIN(D, 4, GPIO_OUT)   // Led-15, OC1B (Timer1 COMPARE_B)
#define LED_RED_PIN     GPIO_PIN(A, 0, GPIO_OUT)   // Led-37
#define LED_GREEN_PIN   GPIO_PIN(A, 1, GPIO_OUT)   // Led-36

/* Not const: GPIO_PIN() is a compound literal, and using one to initialize
 * a static-storage-duration object (a file-scope const) is only a GNU
 * extension - -Wpedantic flags it regardless of -std=c11 vs gnu11. Since
 * btn.h/led.h's functions take non-const pointers anyway, these are set up
 * at runtime by BOARD_Init() instead of via a static initializer. */
extern led_t led1;
extern led_t led_red;
extern led_t led_green;

// Buttons
#define BTN_PIN         GPIO_PIN(D, 0, GPIO_IN)    // Push-30
#define BTN2_PIN        GPIO_PIN(D, 1, GPIO_IN)    // Push-31
#define BTN3_PIN        GPIO_PIN(D, 2, GPIO_IN)    // Push-32

extern btn_t btn1;
extern btn_t btn2;
extern btn_t btn3;

/* Assigns every board_t's .pin (and .pullup, for buttons) at runtime.
 * Call once at the top of main(), before BTN_Init()/LED_Init(). */
void BOARD_Init(void);

#endif