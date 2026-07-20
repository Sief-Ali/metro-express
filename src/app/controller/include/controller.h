#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "controller_types.h"
#include "app_types.h"

void Controller_Init(volatile controller_state_t * current_state);

void Controller_Update(volatile controller_state_t * current_state, volatile extint_flags_t *flags);

void Controller_SetState(controller_state_t current_state);

#endif