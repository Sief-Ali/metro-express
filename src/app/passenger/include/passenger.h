#ifndef PASSENGER_H
#define PASSENGER_H

#include <stdint.h>
#include <stdbool.h>
#include "passenger_types.h"

/* Initializer */
void Passenger_Init(void);

/* Destination Controls */
void Passenger_NextDestination(void);
destination_t Passenger_GetSelectedDestination(void);
const char* Passenger_GetSelectedDestinationName(void);

/* Quantity & Stock Controls */
void Passenger_SetQuantity(uint8_t qty);
uint8_t Passenger_GetQuantity(void);
bool Passenger_HasEnoughStock(uint8_t requested_qty);

/* Ticket & Transaction Controls */
void Passenger_GenerateTicketCode(void);
const char* Passenger_GetTicketCode(void);
bool Passenger_CommitPurchase(void); // Deducts stock & increments sequence number
void Passenger_Reset(void);          // Clears active purchase data for next passenger

#endif /* PASSENGER_H */