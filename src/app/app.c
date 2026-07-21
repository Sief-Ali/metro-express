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

volatile controller_state_t current_state = STATE_IDLE;

volatile extint_flags_t extint_flags = {false}; 

static void OnNextBTNEdge(void) {
  extint_flags.next_pressed = true;
}

static void OnConfirmBTNEdge(void) {
  extint_flags.confirm_pressed = true;
}

static void OnCancelBTNEdge(void) {
  extint_flags.cancel_pressed = true;
}

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

void APP_Run(void)
{
    while (1)
    {
        UI_Update_Page(&current_state);

        Controller_Update(&current_state, &extint_flags);
    }
}