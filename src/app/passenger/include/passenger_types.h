#ifndef PASSENGER_TYPES_H
#define PASSENGER_TYPES_H

#include <stdint.h>

typedef enum {
  // First D1 = 0–2
    ALEXANDRIA=0,
    CAIRO,
    ASWAN,

  // Second D1 = 3–5
    LUXOR,
    HURGADA,
    MARSA_MATROUH,

  // Third D1 = 6–7
    SUEZ,
    TANTA,
    MANSOURA,

  // Fourth D1 = 8–9
    PORT_SAID,
    ISMAILIA,
    DAMANHUR,

    DESTINATION_COUNT
} destination_t;

typedef struct {

  const char *display_name; // Name shown on LCD (e.g., "Alex.")

  const char *code;         // 3-letter uppercase code for tickets (e.g., "ALX")

} destination_info_t;

typedef struct
{
  destination_t destination;
  
  uint8_t quantity;
  
  char ticket_code[16];
  
} passenger_t;

extern const destination_info_t destination_info[DESTINATION_COUNT];

#endif