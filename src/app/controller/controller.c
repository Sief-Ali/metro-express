#include "controller.h"
#include "analog.h"
#include "board.h"
#include "controller_types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <util/delay.h>

#include "app_types.h"
#include "logger.h"
#include "passenger.h"
#include "timer_service.h"
#include "ui.h"

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

// timeout 
static volatile uint16_t inactivity_counter_ms = 0;
#define INACTIVITY_TIMEOUT_MS 15000U // 15 Seconds for D1 = 1

// Timer Callback (Runs in ISR context every 100ms)
static void OnInactivityTick(void) {
    inactivity_counter_ms += 100U;
}

// Helper to start or reset timer on user activity
void Controller_ResetInactivityTimer(void) {
    inactivity_counter_ms = 0;
    // Arm Timer 1 to fire every 100ms
    TIMER_SERVICE_SetInterval(TIMER_1, 100U, OnInactivityTick);
}

// Helper to stop timer when entering IDLE
void Controller_StopInactivityTimer(void) {
    TIMER_SERVICE_Cancel(TIMER_1);
    inactivity_counter_ms = 0;
}

// cancel handler

static void Cancel(bool cancel) {
  if (cancel) {
      Passenger_Reset();
      Controller_StopInactivityTimer();

      last_state = *current_state_ptr;
      *current_state_ptr = STATE_IDLE;
      
      Logger_Log(
        LOG_INFO,
        "[0x00]:Cancel back to Idel");

      UI_SetPage(UI_PAGE_CANCELLED);


      clear_flags();
  }
}

//state handlers

static void Idle_State(void) {
  if (extint_flags_ptr != NULL) {
      bool next = check_and_clear_flag(&extint_flags_ptr->next_pressed);
      bool confirm = check_and_clear_flag(&extint_flags_ptr->confirm_pressed);

      if (next || confirm) {
          Controller_ResetInactivityTimer(); // Resets timer back to 0ms

          Passenger_Reset();

          last_state = *current_state_ptr;
          *current_state_ptr = STATE_SELECT_DESTINATION;

          Logger_Log(
            LOG_INFO,
            "[0x00]:WAKE UP Go Select Destination");

          clear_flags();
      }
  }
}

static void Select_Destination_State(void) {
  if (extint_flags_ptr != NULL) {
      bool next = check_and_clear_flag(&extint_flags_ptr->next_pressed);
      bool confirm = check_and_clear_flag(&extint_flags_ptr->confirm_pressed);

      bool cancel = check_and_clear_flag(&extint_flags_ptr->cancel_pressed);

      if (next) {
          Controller_ResetInactivityTimer(); // Resets timer back to 0ms

          Passenger_NextDestination();
          UI_Update_Destination();
      }

      if (confirm) {
          Controller_ResetInactivityTimer(); // Resets timer back to 0ms

          last_state = *current_state_ptr;
          *current_state_ptr = STATE_SELECT_QUANTITY;

          Logger_Log(
            LOG_INFO,
            "[0x00]:NEXT Go Select Quantity");

          clear_flags();
      }

      Cancel(cancel);
  }
}

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
              Logger_Log(
                LOG_ERROR,
                "[0x00]:Choose qty. Quantity valid between 1 and 5");
              UI_SetPage(UI_PAGE_QUANTITY_NOT_VALID);

              clear_flags();
              return;
          }
          
          last_state = *current_state_ptr;
          *current_state_ptr = STATE_CONFIRMATION;

          Logger_Log(
            LOG_INFO,
            "[0x00]:NEXT Go To Confirmation");

          UI_Update_Summarize();


          Passenger_SetQuantity(current_qty);

          clear_flags();
          return;
      }

      Cancel(cancel);
  }
}

static void Confirmation_State(void) {
  if (extint_flags_ptr != NULL) {
      bool next = check_and_clear_flag(&extint_flags_ptr->next_pressed);
      bool confirm = check_and_clear_flag(&extint_flags_ptr->confirm_pressed);

      bool cancel = check_and_clear_flag(&extint_flags_ptr->cancel_pressed);

      if (next || confirm) {  
          Controller_ResetInactivityTimer(); // Resets timer back to 0ms
    
          last_state = *current_state_ptr;
          *current_state_ptr = STATE_PROCESSING;

          Logger_Log(
            LOG_INFO,
            "[0x00]:NEXT Go To Processing");

          clear_flags();
          return;
      }

      Cancel(cancel);
  }
}

static void Processing_State(void) {
  // When progress/timer finishes:
  if (Passenger_CommitPurchase()) {
      const char *ticket_code = Passenger_GetTicketCode();
      
      UI_SetLed(&led.processing);

      //for testing only simulate ticket printing process
      _delay_ms(1500);

      // Log ticket over UART after processing ticket printing process
      Logger_Log(LOG_INFO, ticket_code);

      *current_state_ptr = STATE_TICKET_ISSUED;
      
      Logger_Log(
        LOG_INFO,
        "[0x00]:Go to Ticket Issued");
      
  } else {
      // Trigger Out-of-Stock Error E02
      UI_SetPage(UI_PAGE_OUT_OF_STOCK);

      *current_state_ptr = STATE_IDLE;
  }
}

static void Ticket_Issued_State(void) {
  if (extint_flags_ptr != NULL) {
      bool next = check_and_clear_flag(&extint_flags_ptr->next_pressed);
      bool confirm = check_and_clear_flag(&extint_flags_ptr->confirm_pressed);

      bool cancel = check_and_clear_flag(&extint_flags_ptr->cancel_pressed);

      if (next || confirm || cancel) {      
          last_state = *current_state_ptr;
          *current_state_ptr = STATE_IDLE;

          Logger_Log(
            LOG_INFO,
            "[0x00]:Back To Idle");

          clear_flags();
          return;
      }
  }
}

// main controls
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

void Controller_Init(volatile controller_state_t * current_state) {
  current_state_ptr = current_state;

  *current_state_ptr = STATE_IDLE;
  last_state = STATE_IDLE;

  Controller_SetState(STATE_IDLE);
}

void Controller_Update(volatile controller_state_t * current_state, volatile extint_flags_t *flags) {
  current_state_ptr = current_state;
  extint_flags_ptr = flags;

  if (inactivity_counter_ms >= INACTIVITY_TIMEOUT_MS) {
      Logger_Log(LOG_INFO, "[0x00]:Timeout 15s reached -> Returning to IDLE");
      
      Passenger_Reset();
      Controller_StopInactivityTimer();
      
      *current_state_ptr = STATE_IDLE;
  }

  // Excellent fix here! Evaluating the actual active state value.
  Controller_SetState(*current_state);
}