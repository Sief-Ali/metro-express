#include "passenger.h"
#include "passenger_types.h"
#include <string.h>
#include <stdio.h>

#define STUDENT_ID "463688431"

/* -------------------------------------------------------------------------- */
/* Dynamic Passenger Configuration                                             */
/* -------------------------------------------------------------------------- */
static destination_t assigned_destinations[3];
static uint8_t stock[3] = {5, 5, 5};
static uint8_t active_city_count = 3;

static uint8_t extracted_d1 = 1;
static uint8_t extracted_d2 = 3;

static uint8_t current_dest_index = 0;
static passenger_t active_transaction;
static uint16_t global_sequence_number = 1; // Starts at 1

/**
 * Parses Student ID to extract D1 (last digit) and D2 (second-to-last digit).
 */
/* Selects destination set and ticket format digits from the student ID. */
static void Passenger_ConfigureFromStudentID(const char *student_id) {
    if (student_id == NULL) return;

    size_t len = strlen(student_id);
    if (len < 2) return; // Needs at least 2 digits

    // Extract D1 and D2 as numeric integers
    extracted_d1 = (uint8_t)(student_id[len - 1] - '0');
    extracted_d2 = (uint8_t)(student_id[len - 2] - '0');

    // Map D1 to assigned destination set
    if (extracted_d1 <= 2) {
        assigned_destinations[0] = ALEXANDRIA;
        assigned_destinations[1] = CAIRO;
        assigned_destinations[2] = ASWAN;
        active_city_count = 3;
    } else if (extracted_d1 <= 5) {
        assigned_destinations[0] = LUXOR;
        assigned_destinations[1] = HURGADA;
        assigned_destinations[2] = MARSA_MATROUH;
        active_city_count = 3;
    } else if (extracted_d1 <= 8) {
        assigned_destinations[0] = SUEZ;
        assigned_destinations[1] = TANTA;
        assigned_destinations[2] = MANSOURA;
        active_city_count = 3;
    } else {
        assigned_destinations[0] = PORT_SAID;
        assigned_destinations[1] = ISMAILIA;
        assigned_destinations[2] = DAMANHUR;
        active_city_count = 3;
    }

    // Reset default stock for assigned stations
    for (uint8_t i = 0; i < active_city_count; i++) {
        stock[i] = 5;
    }
}

/* Initializes destination assignment and clears the first transaction. */
void Passenger_Init(void) {
    Passenger_ConfigureFromStudentID(STUDENT_ID);

    current_dest_index = 0;
    active_transaction.destination = assigned_destinations[current_dest_index];
    active_transaction.quantity = 0;
    active_transaction.ticket_code[0] = '\0';
}

/* Advances to the next assigned destination, wrapping after the third. */
void Passenger_NextDestination(void) {
    current_dest_index = (current_dest_index + 1) % active_city_count;
    active_transaction.destination = assigned_destinations[current_dest_index];
}

/* Returns the currently selected destination enum value. */
destination_t Passenger_GetSelectedDestination(void) {
    return active_transaction.destination;
}

/* Returns the LCD display name for the selected destination. */
const char* Passenger_GetSelectedDestinationName(void) {
    destination_t current = active_transaction.destination;
    return destination_info[current].display_name;
}

/* Stores the quantity currently selected by the passenger. */
void Passenger_SetQuantity(uint8_t qty) {
    active_transaction.quantity = qty;
}

/* Returns the quantity stored in the active transaction. */
uint8_t Passenger_GetQuantity(void) {
    return active_transaction.quantity;
}

/* Checks whether the selected destination can satisfy the request. */
bool Passenger_HasEnoughStock(uint8_t requested_qty) {
    return (requested_qty <= stock[current_dest_index]);
}

/**
 * Dynamically formats the ticket code according to extracted D2 digit.
 */
/* Builds the ticket code string for the current transaction. */
void Passenger_GenerateTicketCode(void) {
    destination_t dest = active_transaction.destination;
    const char *dest_code = destination_info[dest].code;
    uint8_t qty = active_transaction.quantity;

    if (extracted_d2 <= 2) {
        // Format Type 1: <DEST>-<QTY>-<SEQ3> (e.g., "CAI-2-001")
        (void)snprintf(active_transaction.ticket_code, 
          sizeof(active_transaction.ticket_code),
          "%s-%u-%03u", 
          dest_code, 
          qty, 
          global_sequence_number);
                 
    } else if (extracted_d2 <= 6) {
        // Format Type 2: <DEST><SEQ4>#<QTY> (e.g., "CAI0001#2")
        (void)snprintf(active_transaction.ticket_code, 
                 sizeof(active_transaction.ticket_code),
                 "%s%04u#%u", 
                 dest_code, 
                 global_sequence_number, 
                 qty);
                 
    } else {
        // Format Type 3: TKT:<SEQ5>:<DEST>:<QTY> (e.g., "TKT:00001:CAI:2")
        (void)snprintf(active_transaction.ticket_code, 
                 sizeof(active_transaction.ticket_code),
                 "TKT:%05u:%s:%u", 
                 global_sequence_number, 
                 dest_code, 
                 qty);
    }
}

/* Returns the generated ticket code buffer. */
const char* Passenger_GetTicketCode(void) {
    return active_transaction.ticket_code;
}

/* Deducts stock and advances sequence only when enough stock exists. */
bool Passenger_CommitPurchase(void) {
    if (!Passenger_HasEnoughStock(active_transaction.quantity)) {
        return false;
    }

    // Generate ticket string format before incrementing sequence
    Passenger_GenerateTicketCode();

    // Deduct stock and increment sequence number
    stock[current_dest_index] -= active_transaction.quantity;
    global_sequence_number++;
    return true;
}

/* Clears the active transaction and returns selection to the first city. */
void Passenger_Reset(void) {
    current_dest_index = 0;
    active_transaction.destination = assigned_destinations[current_dest_index];
    active_transaction.quantity = 0;
    active_transaction.ticket_code[0] = '\0';
}
