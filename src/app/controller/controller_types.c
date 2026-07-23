#include "controller_types.h"

/* Display names for each state, indexed directly by controller_state_t values */
const char * const controller_state_names[STATE_COUNT] = {
    [STATE_IDLE]               = "STATE_IDLE",
    [STATE_SELECT_DESTINATION] = "STATE_SELECT_DESTINATION",
    [STATE_SELECT_QUANTITY]    = "STATE_SELECT_QUANTITY",
    [STATE_CONFIRMATION]       = "STATE_CONFIRMATION",
    [STATE_PROCESSING]         = "STATE_PROCESSING",
    [STATE_TICKET_ISSUED]      = "STATE_TICKET_ISSUED"
};