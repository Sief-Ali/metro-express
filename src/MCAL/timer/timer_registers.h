#ifndef TIMER_REGISTERS_H
#define TIMER_REGISTERS_H

#include <stdint.h>

#define TIMER_TCCR0 (*(volatile uint8_t*)0x53)
#define TIMER_TCNT0 (*(volatile uint8_t*)0x52)
#define TIMER_OCR0  (*(volatile uint8_t*)0x5C)

#define TIMER_TCCR1A (*(volatile uint8_t*)0x4F)
#define TIMER_TCCR1B (*(volatile uint8_t*)0x4E)
#define TIMER_TCNT1  (*(volatile uint16_t*)0x4C)
#define TIMER_OCR1A  (*(volatile uint16_t*)0x4A)

#define TIMER_TCCR2 (*(volatile uint8_t*)0x45)
#define TIMER_TCNT2 (*(volatile uint8_t*)0x44)
#define TIMER_OCR2  (*(volatile uint8_t*)0x43)

#define TIMER_TIFR  (*(volatile uint8_t*)0x58)

#endif
