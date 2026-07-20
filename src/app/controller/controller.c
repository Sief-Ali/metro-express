#include "controller.h"
#include "controller_types.h"

#include <stddef.h>

#include "app_types.h"
#include "logger.h"

static volatile controller_state_t last_state;
static volatile controller_state_t *current_state_ptr = NULL;

static volatile extint_flags_t *extint_flags_ptr = NULL;

static void Idle_State(void) {
  // Check if the pointer is not NULL first to prevent crashing
  if (extint_flags_ptr != NULL) {
      
      // Correct syntax using the arrow operator
      if (extint_flags_ptr->next_pressed || extint_flags_ptr->confirm_pressed) {
          last_state = *current_state_ptr;
          *current_state_ptr = STATE_SELECT_DESTINATION;

          Logger_Log(
            LOG_INFO,
            "[0x00]:WAKE UP Go Select Destination page");

          extint_flags_ptr->next_pressed = false;
          extint_flags_ptr->confirm_pressed = false;
      }
      
  }
}

static void Select_Destination_State(void) {
  // Check if the pointer is not NULL first to prevent crashing
  if (extint_flags_ptr != NULL) {
      
      // Correct syntax using the arrow operator
      if (extint_flags_ptr->cancel_pressed) {
          last_state = *current_state_ptr;
          *current_state_ptr = STATE_IDLE;
          
          Logger_Log(
            LOG_INFO,
            "[0x00]:Cancel back to ideal");        

          extint_flags_ptr->cancel_pressed = false;
      }
      
  }
}


void Controller_SetState(controller_state_t current_state) {
  switch (current_state) {
    case STATE_IDLE:
        Idle_State();
        break;
    case STATE_SELECT_DESTINATION:
        Select_Destination_State();
        break;
    default:
        Idle_State();
        break;
  }
}

void Controller_Init(volatile controller_state_t * current_state) {
  current_state_ptr = current_state;

  *current_state_ptr = STATE_IDLE;
  last_state = STATE_IDLE;

  Controller_SetState(STATE_IDLE);
}

void Controller_Update(volatile controller_state_t * current_state, volatile extint_flags_t *flags) {

  current_state_ptr = current_state;
  extint_flags_ptr = flags;

  Controller_SetState(*current_state);
}