#include "app.h"
#include "app_types.h"

#include <stdbool.h>

#include "controller.h"
#include "controller_types.h"
#include "ui.h"
#include "logger.h"

controller_state_t current_state = STATE_IDLE;

extint_flags_t extint_flags = {false}; 

void APP_Init(void)
{

    UI_SetPage(UI_PAGE_IDLE);

    Controller_Init(&current_state);

    /* Register interrupts */

    /* Initialize controller */

    /* Initialize passenger */

    /* Initialize error system */

    Logger_Log(
      LOG_INFO,
      "[0x00]:SYS_INIT_OK");

}

void APP_Run(void)
{
    while (1)
    {
        UI_Update(current_state);

        Controller_Update(&current_state, &extint_flags);

        /* Read flags */

        /* Update controller */

        /* Housekeeping */
    }
}