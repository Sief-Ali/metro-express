#include "ui.h"

#include "lcd.h"
#include "lcd_config.h"

static void ui_idle(void) {
  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  /* Center of a 16x2 LCD:
   * "Hello World!" = 14 chars
   * (16 - 14) / 2 = column 1
   */
  LCD_SetCursor(&lcd_display, 0, 1);
  LCD_PrintString(&lcd_display, "Metro Express");
}

void UI_ShowState(ui_state_t state) {
  switch (state) {
    case UI_STATE_IDLE:
        ui_idle();
        break;
    default:
        ui_idle();
        break;
  }
}