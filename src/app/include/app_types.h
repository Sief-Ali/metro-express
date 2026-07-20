#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdbool.h>

typedef struct
{
    volatile bool next_pressed;
    volatile bool confirm_pressed;
    volatile bool cancel_pressed;

} extint_flags_t;


#endif