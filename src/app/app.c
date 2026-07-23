#include "app.h"
#include "app_types.h"

#include <stdbool.h>

#include "board.h"
#include "btn.h"
#include "controller.h"
#include "controller_types.h"
#include "ext_int.h"
#include "passenger.h"
#include "ui.h"
#include "logger.h"

/* Global state shared by the main loop and controller state machine. */
volatile controller_state_t current_state = STATE_IDLE;

/* Button interrupt flags are set in ISRs and consumed in the main loop. */
volatile extint_flags_t extint_flags = {false}; 

/* Records a NEXT press from the button interrupt callback. */
static void OnNextBTNEdge(void) {
  extint_flags.next_pressed = true;
}

/* Records a CONFIRM press from the button interrupt callback. */
static void OnConfirmBTNEdge(void) {
  extint_flags.confirm_pressed = true;
}

/* Records a CANCEL press from the button interrupt callback. */
static void OnCancelBTNEdge(void) {
  extint_flags.cancel_pressed = true;
}

/* Initializes passenger data, UI, button interrupts, and the controller. */
void APP_Init(void)
{

    BTN_InitInterrupt(
        &btn.next,
        EXT_INT_TRIGGER_FALLING_EDGE,
        OnNextBTNEdge);

    BTN_InitInterrupt(
        &btn.confirm,
        EXT_INT_TRIGGER_FALLING_EDGE,
        OnConfirmBTNEdge);

    BTN_InitInterrupt(
        &btn.cancel,
        EXT_INT_TRIGGER_FALLING_EDGE,
        OnCancelBTNEdge);

    EXT_INT_GlobalEnable();

    Passenger_Init();
    
    UI_SetPage(UI_PAGE_IDLE);

    Controller_Init(&current_state);

    Logger_Log(
      LOG_INFO,
      "[0x00]:SYS_INIT_OK");

}

/* Runs the foreground loop: redraw UI, then process controller behavior. */
void APP_Run(void)
{
    while (1)
    {
        UI_Update_Page(&current_state);

        Controller_Update(&current_state, &extint_flags);
    }
}
