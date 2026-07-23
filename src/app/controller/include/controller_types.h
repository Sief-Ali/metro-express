#ifndef CONTROLLER_TYPES_H
#define CONTROLLER_TYPES_H

/* Steps in the MetroExpress ticket-purchase state machine. */
typedef enum
{
    STATE_IDLE,

    STATE_SELECT_DESTINATION,

    STATE_SELECT_QUANTITY,

    STATE_CONFIRMATION,

    STATE_PROCESSING,

    STATE_TICKET_ISSUED

} controller_state_t;



#endif
