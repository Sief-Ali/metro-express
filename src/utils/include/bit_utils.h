#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#define SET_BIT(REG,BIT)      ((REG) |=  (uint8_t)(1U << (BIT)))
#define CLEAR_BIT(REG,BIT)    ((REG) &= (uint8_t) ~(1U << (BIT)))
#define TOGGLE_BIT(REG,BIT)   ((REG) ^=  (uint8_t)(1U << (BIT)))
#define READ_BIT(REG,BIT)     (((REG) >> (BIT)) & 1U)

#endif