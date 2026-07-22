#include "passenger.h"
#include "passenger_types.h"
#include <string.h>
#include <stdio.h>

#define DEFAULT_STUDENT_ID "463688436"

/* Dynamic Configuration Variables */
static destination_t assigned_destinations[3];
static uint8_t stock[3] = {5, 5, 5};
static uint8_t active_city_count = 3;

static uint8_t current_dest_index = 0;
static passenger_t active_transaction;
static uint16_t global_sequence_number = 1;

/**
 * Extracts D1 (last numeric digit) from student ID string and maps 
 * the corresponding destination array dynamically.
 */
static void Passenger_ConfigureFromStudentID(const char *student_id) {
    if (student_id == NULL) return;

    size_t len = strlen(student_id);
    if (len == 0) return;

    // Extract last character as integer D1
    uint8_t d1 = (uint8_t)(student_id[len - 1] - '0');

    // Map D1 to assigned cities per assignment table
    if (d1 <= 2) {
        // Group 0, 1, 2
        assigned_destinations[0] = ALEXANDRIA;
        assigned_destinations[1] = CAIRO;
        assigned_destinations[2] = ASWAN;
        active_city_count = 3;
    } else if (d1 <= 5) {
        // Group 3, 4, 5
        assigned_destinations[0] = LUXOR;
        assigned_destinations[1] = HURGADA;
        assigned_destinations[2] = MARSA_MATROUH;
        active_city_count = 3;
    } else if (d1 <= 8) {
        // Group 6, 7, 8
        assigned_destinations[0] = SUEZ;
        assigned_destinations[1] = TANTA;
        assigned_destinations[2] = MANSOURA;
        active_city_count = 3;
    } else {
        // Group 9
        assigned_destinations[0] = PORT_SAID;
        assigned_destinations[1] = ISMAILIA;
        assigned_destinations[2] = DAMANHUR;
        active_city_count = 3;
    }

    // Initialize initial stock for all active assigned stations
    for (uint8_t i = 0; i < active_city_count; i++) {
        stock[i] = 5;
    }
}

void Passenger_Init(void) {
    // Dynamically parse student ID (can easily accept RFID string later)
    Passenger_ConfigureFromStudentID(DEFAULT_STUDENT_ID);

    current_dest_index = 0;
    active_transaction.destination = assigned_destinations[current_dest_index];
    active_transaction.quantity = 0;
    active_transaction.ticket_code[0] = '\0';
}

void Passenger_NextDestination(void) {
    // Dynamic modulo arithmetic based on active city count
    current_dest_index = (current_dest_index + 1) % active_city_count;
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
    current_dest_index = 0;
    active_transaction.destination = assigned_destinations[current_dest_index];
    active_transaction.quantity = 0;
    active_transaction.ticket_code[0] = '\0';
}