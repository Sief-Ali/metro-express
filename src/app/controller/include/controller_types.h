#ifndef CONTROLLER_TYPES_H
#define CONTROLLER_TYPES_H

/* Steps in the MetroExpress ticket-purchase state machine. */
typedef enum
{
    STATE_IDLE = 0,

    STATE_SELECT_DESTINATION,

    STATE_SELECT_QUANTITY,

    STATE_CONFIRMATION,

    STATE_PROCESSING,

    STATE_TICKET_ISSUED,

    STATE_COUNT

} controller_state_t;

/* Array mapping each state enum value to its human-readable string name. */
extern const char * const controller_state_names[STATE_COUNT];

#endif
