#ifndef TIMER_PRIVATE_H
#define TIMER_PRIVATE_H

/* TCCR0 bits */
#define TIMER_FOC0   7U
#define TIMER_WGM01  3U
#define TIMER_WGM00  6U

/* TCCR1A bits */
#define TIMER_FOC1A  3U
#define TIMER_WGM10  0U
#define TIMER_WGM11  1U

/* TCCR1B bits */
#define TIMER_WGM12  3U
#define TIMER_WGM13  4U

/* TCCR2 bits */
#define TIMER_FOC2   7U
#define TIMER_WGM21  3U
#define TIMER_WGM20  6U

/* TIFR bits */
#define TIMER_OCF0   1U
#define TIMER_OCF1A  4U
#define TIMER_OCF2   7U

#endif
