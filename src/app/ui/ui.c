#include "ui.h"

#include "lcd.h"
#include "lcd_config.h"
#include "controller_types.h"

static void UI_Idle(void) {
  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  /* Center of a 16x2 LCD:
   * "Metro Express" = 14 chars
   * (16 - 14) / 2 = column 1
   */
  LCD_SetCursor(&lcd_display, 0, 1);
  LCD_PrintString(&lcd_display, "Metro Express");
}

static void UI_Select_Destination(void) {
  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  /* Center of a 16x2 LCD:
   * "Select Destinat" = 16 chars
   * (16 - 16) / 2 = error //just keep it 0
   */
  LCD_SetCursor(&lcd_display, 0, 0);
  LCD_PrintString(&lcd_display, "Select Destinat");
}

void UI_SetPage(ui_page_t state) {
  switch (state) {
    case UI_PAGE_IDLE:
        UI_Idle();
        break;
    case UI_PAGE_SELECT_DESTINATION:
        UI_Select_Destination();
        break;
    default:
        UI_Idle();
        break;
  }
}

void UI_Update(controller_state_t current_state) {
  ui_page_t target_page;
  
  switch (current_state) {
    case STATE_IDLE:
        target_page = UI_PAGE_IDLE;
      break;
    case STATE_SELECT_DESTINATION:
        target_page = UI_PAGE_SELECT_DESTINATION;
      break;
    default:
        target_page = UI_PAGE_IDLE;
      break;
  }

  UI_SetPage(target_page);
}