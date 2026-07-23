#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "controller_types.h"
#include "app_types.h"

/* Initializes the controller state machine and places it in idle. */
void Controller_Init(volatile controller_state_t * current_state);

/* Processes input flags, timeout handling, and the active controller state. */
void Controller_Update(volatile controller_state_t * current_state, volatile extint_flags_t *flags);

/* Runs the handler for the supplied controller state. */
void Controller_SetState(controller_state_t current_state);

#endif