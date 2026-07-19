#include "app.h"

#include "ui.h"
#include "logger.h"

void APP_Init(void)
{
    UI_SetPage(UI_PAGE_IDLE);

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
        UI_Update();
        /* Read flags */

        /* Update controller */

        /* Housekeeping */
    }
}