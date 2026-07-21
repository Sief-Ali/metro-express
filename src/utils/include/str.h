#ifndef STR_H
#define STR_H

#include <stdint.h>
#include <stdlib.h>

// Utility function to convert numbers to char[] automatically
const char* Str(uint8_t value) {
    static char buf[4]; // Static memory stays alive after the function returns
    utoa(value, buf, 10);
    return buf;
}

#endif