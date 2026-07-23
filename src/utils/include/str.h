#ifndef STR_H
#define STR_H

#include <stdint.h>
#include <stdlib.h>

/* Converts an 8-bit value to a decimal string backed by a static buffer. */
static inline const char* Str(uint8_t value) {
    static char buf[4];
    utoa(value, buf, 10);
    return buf;
}

#endif
