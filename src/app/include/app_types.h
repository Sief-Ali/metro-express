#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdbool.h>

/* Input flags written by interrupt callbacks and read by the controller. */
typedef struct
{
    volatile bool next_pressed;
    volatile bool confirm_pressed;
    volatile bool cancel_pressed;

} extint_flags_t;


#endif
