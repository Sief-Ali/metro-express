#include "passenger.h"
#include <stdio.h>
#include <string.h>

#define DEFAULT_STUDENT_ID "463688431"

// Assigned 3 cities for D1 = 1 (Alexandria, Cairo, Aswan)
static const destination_t assigned_destinations[3] = {
    ALEXANDRIA,
    CAIRO,
    ASWAN
};

static uint8_t current_dest_index = 0; // 0..2
static passenger_t active_transaction;
static uint16_t global_sequence_number = 1; // Starts at 1

// Stock for each assigned station (starts at 5 tickets each)
static uint8_t stock[3] = {5, 5, 5};

void Passenger_Init(void) {
    current_dest_index = 0;
    active_transaction.destination = assigned_destinations[current_dest_index];
    active_transaction.quantity = 0;
    active_transaction.ticket_code[0] = '\0';
}

void Passenger_NextDestination(void) {
    // Cycle through assigned cities (0 -> 1 -> 2 -> 0)
    current_dest_index = (current_dest_index + 1) % 3;
    active_transaction.destination = assigned_destinations[current_dest_index];
}

destination_t Passenger_GetSelectedDestination(void) {
    return active_transaction.destination;
}

const char* Passenger_GetSelectedDestinationName(void) {
    destination_t current = active_transaction.destination;
    return destination_info[current].display_name;
}

void Passenger_SetQuantity(uint8_t qty) {
    active_transaction.quantity = qty;
}

uint8_t Passenger_GetQuantity(void) {
    return active_transaction.quantity;
}

bool Passenger_HasEnoughStock(uint8_t requested_qty) {
    return (requested_qty <= stock[current_dest_index]);
}

void Passenger_GenerateTicketCode(void) {
    destination_t dest = active_transaction.destination;
    const char *dest_code = destination_info[dest].code;
    uint8_t qty = active_transaction.quantity;

    // D2 = 3 format: <DEST><SEQ4>#<QTY> (e.g., "CAI0001#2")
    // snprintf(active_transaction.ticket_code, 
    //          sizeof(active_transaction.ticket_code),
    //          "%s%04u#%u", 
    //          dest_code, 
    //          global_sequence_number, 
    //          qty);
}

const char* Passenger_GetTicketCode(void) {
    return active_transaction.ticket_code;
}

bool Passenger_CommitPurchase(void) {
    if (!Passenger_HasEnoughStock(active_transaction.quantity)) {
        return false;
    }

    // Deduct stock and increment sequence number
    stock[current_dest_index] -= active_transaction.quantity;
    global_sequence_number++;
    return true;
}

void Passenger_Reset(void) {
    Passenger_Init();
}