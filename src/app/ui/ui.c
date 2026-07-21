#include "ui.h"

#include "board.h"
#include "lcd.h"
#include "lcd_config.h"
#include "controller_types.h"
#include "led.h"

static controller_state_t last_state;

/*
  TODO: Refactor to adhere to DRY principles and use a component-based design with dedicated page title fields, rather than hardcoding UI text
*/

// i don't no why i made it so complex but i like it this way

static led_t * const board_led_list[] = {
  &led.ready,
  &led.processing,
  &led.ticket,
  &led.error
};

static void setLedOn(led_t * led){
  int led_length = (sizeof(board_led_list) / sizeof(board_led_list[0])) - 1;
  for (int index = 0; index < led_length ; index++) {
    LED_Off(board_led_list[index]);
  }
  
  LED_On(led);
}

static void UI_Idle(void) {
  LCD_Init(&lcd_display);

  LCD_Clear(&lcd_display);

  /* Center of a 16x2 LCD:
   * "Metro Express" = 14 chars
   * (16 - 14) / 2 = column 1
   */
  LCD_SetCursor(&lcd_display, 0, 1);
  LCD_PrintString(&lcd_display, "Metro Express");

  setLedOn(&led.ready);
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

  setLedOn(&led.ready);
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

void UI_Update(volatile controller_state_t * current_state) {

  if (last_state == *current_state) return;
  
  last_state = *current_state;

  ui_page_t target_page;
  
  switch (*current_state) {
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