#include "controller.h"
#include "controller_types.h"

#include <stdbool.h>
#include <stddef.h>

#include "app_types.h"
#include "logger.h"

static volatile controller_state_t last_state;
static volatile controller_state_t *current_state_ptr = NULL;
static volatile extint_flags_t *extint_flags_ptr = NULL;

static void clear_flags(void){
  extint_flags_ptr->next_pressed = false;
  extint_flags_ptr->confirm_pressed = false;
  extint_flags_ptr->cancel_pressed = false;
}

static bool check_and_clear_flag(volatile bool *flag) {
    if (flag != NULL && *flag == true) {
        *flag = false; // Clear it immediately
        return true;   // Tell the caller it was active
    }
    return false;
}

static void Idle_State(void) {
  if (extint_flags_ptr != NULL) {
      bool next = check_and_clear_flag(&extint_flags_ptr->next_pressed);
      bool confirm = check_and_clear_flag(&extint_flags_ptr->confirm_pressed);

      if (next || confirm) {
          last_state = *current_state_ptr;
          *current_state_ptr = STATE_SELECT_DESTINATION;

          Logger_Log(
            LOG_INFO,
            "[0x00]:WAKE UP Go Select Destination page");

          clear_flags();
      }
  }
}

static void Select_Destination_State(void) {
  if (extint_flags_ptr != NULL) {
      bool cancel = check_and_clear_flag(&extint_flags_ptr->cancel_pressed);

      if (cancel) {
          last_state = *current_state_ptr;
          *current_state_ptr = STATE_IDLE;
          
          Logger_Log(
            LOG_INFO,
            "[0x00]:Cancel back to ideal");

          clear_flags();
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

  // Excellent fix here! Evaluating the actual active state value.
  Controller_SetState(*current_state);
}