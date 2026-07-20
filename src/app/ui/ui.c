#include "ui.h"

#include "lcd.h"
#include "lcd_config.h"
#include "controller_types.h"

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

void UI_SetPage(ui_page_t state) {
  switch (state) {
    case UI_PAGE_IDLE:
        ui_idle();
        break;
    default:
        ui_idle();
        break;
  }
}

void UI_Update(controller_state_t current_state) {
  ui_page_t target_page;
  
  switch (current_state) {
    case STATE_IDLE:
        target_page = UI_PAGE_IDLE;
      break;
    default:
        target_page = UI_PAGE_IDLE;
      break;
  }

  UI_SetPage(target_page);
}