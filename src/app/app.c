#include "app.h"
#include "lcd.h"
#include "lcd_config.h"
#include "logger.h"

void APP_Init(void)
{

  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  /* Center of a 16x2 LCD:
   * "Hello World!" = 14 chars
   * (16 - 14) / 2 = column 1
   */
  LCD_SetCursor(&lcd_display, 0, 1);
  LCD_PrintString(&lcd_display, "Metro Express");
  
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
        /* Read flags */

        /* Update controller */

        /* Housekeeping */
    }
}