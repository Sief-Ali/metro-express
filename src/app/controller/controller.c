#include "controller.h"
#include "analog.h"
#include "board.h"
#include "controller_types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>

#include "app_types.h"
#include "logger.h"
#include "passenger.h"
#include "str.h"
#include "timer_service.h"
#include "ui.h"

/* Controller context shared by state handlers during one update pass. */
static volatile controller_state_t last_state;
static volatile controller_state_t *current_state_ptr = NULL;
static volatile extint_flags_t *extint_flags_ptr = NULL;

/* Clears all pending button flags after a transition consumes them. */
static void clear_flags(void){
  extint_flags_ptr->next_pressed = false;
  extint_flags_ptr->confirm_pressed = false;
  extint_flags_ptr->cancel_pressed = false;
}

/* Returns true once for a pending flag and clears it immediately. */
static bool check_and_clear_flag(volatile bool *flag) {
    if (flag != NULL && *flag == true) {
        *flag = false; // Clear it immediately
        return true;   // Tell the caller it was active
    }
    return false;
}

// Inactivity timeout handling
static volatile uint16_t inactivity_counter_ms = 0;
#define INACTIVITY_TIMEOUT_MS 15000U // 15 Seconds for D1 = 1

// Timer Callback (Runs in ISR context every 100ms)
/* Timer callback that accumulates inactivity time in 100 ms steps. */
static void OnInactivityTick(void) {
    inactivity_counter_ms += 100U;
}

// Helper to start or reset timer on user activity
/* Restarts the inactivity counter after user activity. */
void Controller_ResetInactivityTimer(void) {
    inactivity_counter_ms = 0;
    // Arm Timer 1 to fire every 100ms
    TIMER_SERVICE_SetInterval(TIMER_1, 100U, OnInactivityTick);
}

// Helper to stop timer when entering IDLE
/* Stops the inactivity timer when the flow no longer needs it. */
void Controller_StopInactivityTimer(void) {
    TIMER_SERVICE_Cancel(TIMER_1);
    inactivity_counter_ms = 0;
}

static void Handle_Timeout(void) {
  Passenger_Reset();
  Controller_StopInactivityTimer();

  last_state = *current_state_ptr;
  *current_state_ptr = STATE_IDLE;

  // Timeout log: "timeout from=STATE_SELECT_DESTINATION next=STATE_IDLE"
  char timeout_msg[48] = "timeout from=";
  Str_Cat(timeout_msg, controller_state_names[last_state], sizeof(timeout_msg));
  Str_Cat(timeout_msg, " next=STATE_IDLE", sizeof(timeout_msg));

  Logger_Log(LOG_EVENT, timeout_msg);

  UI_SetPage(UI_PAGE_IDLE);
  clear_flags();
}

/* Handles a consumed cancel request by resetting the active transaction. */
static void Cancel(bool cancel) {
  if (cancel) {
      Passenger_Reset();
      Controller_StopInactivityTimer();

      last_state = *current_state_ptr;
      *current_state_ptr = STATE_IDLE;

      // Cancel log: "Cancel from=STATE_SELECT_QUANTITY"
      char cancel_msg[32] = "Cancel from=";
      Str_Cat(cancel_msg, controller_state_names[last_state], sizeof(cancel_msg));

      Logger_Log(LOG_EVENT, cancel_msg);

      UI_SetPage(UI_PAGE_CANCELLED);
      clear_flags();
  }
}

/* -------------------------------------------------------------------------- */
/* State Handlers                                                              */
/* -------------------------------------------------------------------------- */

/* Waits for the first button press and starts destination selection. */
static void Idle_State(void) {
  if (extint_flags_ptr != NULL) {
      bool next = check_and_clear_flag(&extint_flags_ptr->next_pressed);
      bool confirm = check_and_clear_flag(&extint_flags_ptr->confirm_pressed);

      if (next || confirm) {
          Controller_ResetInactivityTimer(); // Resets timer back to 0ms

          Passenger_Reset();

          last_state = *current_state_ptr;
          *current_state_ptr = STATE_SELECT_DESTINATION;

          clear_flags();
      }
  }
}

/* Handles destination cycling, confirmation, and cancellation. */
static void Select_Destination_State(void) {
  if (extint_flags_ptr != NULL) {
      bool next = check_and_clear_flag(&extint_flags_ptr->next_pressed);
      bool confirm = check_and_clear_flag(&extint_flags_ptr->confirm_pressed);

      bool cancel = check_and_clear_flag(&extint_flags_ptr->cancel_pressed);

      if (next) {
          Controller_ResetInactivityTimer(); // Resets timer back to 0ms

          Passenger_NextDestination();
          UI_Update_Destination();

          const char *dest = Passenger_GetSelectedDestinationName();

          char message[32] = "choose_destination";

          // Dynamically appends " dest=<CurrentDestination>" (e.g. "choose_destination dest=Cairo")
          Str_CatKV(message, "dest", dest, sizeof(message));

          Logger_Log(LOG_EVENT, message);
      }

      if (confirm) {
          Controller_ResetInactivityTimer(); // Resets timer back to 0ms

          last_state = *current_state_ptr;
          *current_state_ptr = STATE_SELECT_QUANTITY;

          clear_flags();
      }

      Cancel(cancel);
  }
}

/* Tracks the dial quantity and validates it before confirmation. */
static void Select_Quantity_State(void) {
  if (extint_flags_ptr != NULL) {
      bool next = check_and_clear_flag(&extint_flags_ptr->next_pressed);
      bool confirm = check_and_clear_flag(&extint_flags_ptr->confirm_pressed);

      bool cancel = check_and_clear_flag(&extint_flags_ptr->cancel_pressed);

      uint8_t current_qty = Analog_Get_Quantity();
      uint8_t last_qty = Passenger_GetQuantity();

      if (current_qty != last_qty) {
          Controller_ResetInactivityTimer(); // Resets timer back to 0ms

          Passenger_SetQuantity(current_qty);
          UI_Update_Quantity();
      }

      if (next || confirm) {
          Controller_ResetInactivityTimer(); // Resets timer back to 0ms
          if (current_qty <= 0U) {
              char message[32] = "[ERR] code=E01 msg=invalid_qty";

              const char* qty = Str_U8(Passenger_GetQuantity());
        
              Str_CatKV(message, "value", qty, sizeof(message));
        
              Logger_Log(LOG_EVENT, message);

              UI_SetPage(UI_PAGE_QUANTITY_NOT_VALID);

              clear_flags();
              return;
          }
          
          last_state = *current_state_ptr;
          *current_state_ptr = STATE_CONFIRMATION;

          UI_Update_Summarize();


          Passenger_SetQuantity(current_qty);

          clear_flags();
          return;
      }

      Cancel(cancel);
  }
}

/* Waits for final confirmation before ticket processing begins. */
static void Confirmation_State(void) {
  if (extint_flags_ptr != NULL) {
      bool next = check_and_clear_flag(&extint_flags_ptr->next_pressed);
      bool confirm = check_and_clear_flag(&extint_flags_ptr->confirm_pressed);

      bool cancel = check_and_clear_flag(&extint_flags_ptr->cancel_pressed);

      if (next || confirm) {  
          Controller_ResetInactivityTimer(); // Resets timer back to 0ms
    
          last_state = *current_state_ptr;
          *current_state_ptr = STATE_PROCESSING;

          clear_flags();
          return;
      }

      Cancel(cancel);
  }
}

/* Commits stock, emits the ticket code, and moves to the result state. */
static void Processing_State(void) {
  // When progress/timer finishes:
  if (Passenger_CommitPurchase()) {      
      UI_SetLed(&led.processing);

      //for testing only simulate ticket printing process
      for (volatile uint32_t i=0;i<40000;i++);;

      char message[48] = "purchase";

      // Build: "purchase dest=Tanta qty=2 code=TAN0007#2"
      Str_CatKV(message, "dest", Passenger_GetSelectedDestinationName(), sizeof(message));
      Str_CatKVNum(message, "qty", Passenger_GetQuantity(), sizeof(message));
      Str_CatKV(message, "code", Passenger_GetTicketCode(), sizeof(message));
  
      // Pass directly to logger
      Logger_Log(LOG_EVENT, message);

      Controller_ResetInactivityTimer();

      *current_state_ptr = STATE_TICKET_ISSUED;
      
  } else {

      char message[32] = "[ERR] code=E02 msg=out_of_stock";

      Logger_Log(LOG_EVENT, message);

      // Trigger Out-of-Stock Error E02
      UI_SetPage(UI_PAGE_OUT_OF_STOCK);

      *current_state_ptr = STATE_IDLE;
  }
}

/* Leaves the issued-ticket page after any button press. */
static void Ticket_Issued_State(void) {
  if (extint_flags_ptr != NULL) {
      bool next = check_and_clear_flag(&extint_flags_ptr->next_pressed);
      bool confirm = check_and_clear_flag(&extint_flags_ptr->confirm_pressed);

      bool cancel = check_and_clear_flag(&extint_flags_ptr->cancel_pressed);

      if (next || confirm || cancel) {      
          last_state = *current_state_ptr;
          *current_state_ptr = STATE_IDLE;

          // Log ticket over UART after processing ticket printing process
          Logger_Log(LOG_INFO, "Back to idle");

          clear_flags();
          return;
      }
  }
}

/* -------------------------------------------------------------------------- */
/* Public Controller Controls                                                  */
/* -------------------------------------------------------------------------- */
/* Dispatches one controller state to its matching handler. */
void Controller_SetState(controller_state_t current_state) {
  switch (current_state) {
    case STATE_IDLE:
        Idle_State();
      break;
    case STATE_SELECT_DESTINATION:
        Select_Destination_State();
      break;
    case STATE_SELECT_QUANTITY:
        Select_Quantity_State();
      break;
    
    case STATE_CONFIRMATION:
        Confirmation_State();
      break;
    case STATE_PROCESSING:
        Processing_State();
      break;
    case STATE_TICKET_ISSUED:
        Ticket_Issued_State();
      break;
    default:
        Idle_State();
      break;
  }
}

/* Stores the state pointer and initializes the controller in idle. */
void Controller_Init(volatile controller_state_t * current_state) {
  current_state_ptr = current_state;

  *current_state_ptr = STATE_IDLE;
  last_state = STATE_IDLE;

  Controller_SetState(STATE_IDLE);
}

/* Processes timeout logic and runs the active state handler once. */
void Controller_Update(volatile controller_state_t * current_state, volatile extint_flags_t *flags) {
  current_state_ptr = current_state;
  extint_flags_ptr = flags;

  // Check for 15-second inactivity timeout across all active states
  if (*current_state != STATE_IDLE && inactivity_counter_ms >= INACTIVITY_TIMEOUT_MS) {
    Handle_Timeout();
    return;
  }

  Controller_SetState(*current_state);
}
